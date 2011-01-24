#include <QStringList>

#include <pulse/context.h>
#include <pulse/glib-mainloop.h>
#include <pulse/introspect.h>
#include <pulse/operation.h>
#include <pulse/subscribe.h>

#include "pacontroller.h"
#include "paoperation.h"


PAController::PAController(QObject *parent) :
    QObject(parent),
    combineMod(PA_INVALID_INDEX),
    oldCombineMod(PA_INVALID_INDEX),
    inputToBeMoved(PA_INVALID_INDEX),
    inputModel(new SinkInputModel(this)),
    sinkModel(new SinkModel(this))
{
    /* initialize PulseAudio data structures */
    mainloop = pa_glib_mainloop_new(NULL);
    context = pa_context_new(pa_glib_mainloop_get_api(mainloop), "conductor");
}

PAController::~PAController()
{
    /* unload all modules previously loaded by us */
    UnloadModuleOperation *o;
    if (combineMod != PA_INVALID_INDEX) {
        o = new UnloadModuleOperation(combineMod, this);
        o->exec(context);
    }
    foreach (uint32_t idx, tunnelModules) {
        o = new UnloadModuleOperation(idx, this);
        o->exec(context);
    }

    /* disconnect and release resources */
    pa_context_disconnect(context);
    pa_context_unref(context);
    pa_glib_mainloop_free(mainloop);
}

void PAController::connectToDaemon(const QByteArray &server)
{
    pa_context_set_state_callback(context, PAController::stateCallback, this);

    /* connect to the daemon */
    if (pa_context_connect(context, server.isEmpty() ? NULL : server.constData(), PA_CONTEXT_NOFAIL, NULL) < 0)
        emit error(tr("pa_context_connect() failed"));
}

void PAController::stateCallback(pa_context *c, void *userdata)
{
    PAController *self = static_cast<PAController*>(userdata);

    switch (pa_context_get_state(c)) {

    case PA_CONTEXT_READY: {
        emit self->connected(pa_context_get_server(c), pa_context_is_local(c));

        /* subscribe to client, sink and sink input events */
        pa_context_set_subscribe_callback(c, PAController::subscribeCallback, self);
        pa_operation *op = pa_context_subscribe(c, (pa_subscription_mask_t)
                                                (PA_SUBSCRIPTION_MASK_CLIENT |
                                                 PA_SUBSCRIPTION_MASK_SINK |
                                                 PA_SUBSCRIPTION_MASK_SINK_INPUT),
                                                NULL, NULL);
        if (op)
            pa_operation_unref(op);
        else
            emit self->error(tr("pa_context_subscribe() failed"));

        /* initialize table models */
        if (!self->inputModel->populate(c))
            emit self->error(tr("failed to populate SinkInputModel"));
        if (!self->sinkModel->populate(c))
            emit self->error(tr("failed to populate SinkModel"));
        break;
    }

    case PA_CONTEXT_FAILED:
        emit self->error(tr("failed to connect to PulseAudio daemon"));
        break;

    default:
        break;
    }
}

void PAController::subscribeCallback(pa_context *c, pa_subscription_event_type_t t, uint32_t idx, void *userdata)
{
    PAController *self = static_cast<PAController*>(userdata);

    /* dispatch the event to the appropriate model for further processing */
    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {

    case PA_SUBSCRIPTION_EVENT_CLIENT:
        switch (t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) {
        case PA_SUBSCRIPTION_EVENT_NEW:
        case PA_SUBSCRIPTION_EVENT_CHANGE:
            self->inputModel->updateClient(c, idx);
            break;
        case PA_SUBSCRIPTION_EVENT_REMOVE:
            self->inputModel->removeClient(idx);
            break;
        }
        break;

    case PA_SUBSCRIPTION_EVENT_SINK:
        switch (t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) {
        case PA_SUBSCRIPTION_EVENT_NEW:
        case PA_SUBSCRIPTION_EVENT_CHANGE:
            self->sinkModel->updateSink(c, idx);
            break;
        case PA_SUBSCRIPTION_EVENT_REMOVE:
            self->sinkModel->removeSink(idx);
            break;
        }
        break;

    case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
        switch (t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) {
        case PA_SUBSCRIPTION_EVENT_NEW:
        case PA_SUBSCRIPTION_EVENT_CHANGE:
            self->inputModel->updateSinkInput(c, idx);
            break;
        case PA_SUBSCRIPTION_EVENT_REMOVE:
            self->inputModel->removeSinkInput(idx);
            break;
        }
        break;
    }
}

void PAController::createTunnel(const QByteArray &server)
{
    if (server.trimmed().isEmpty())
        return;

    /* build the list of arguments to module-tunnel-sink */
    QStringList args;
    args << server.trimmed().prepend("server=");

    /* load module-tunnel-sink */
    LoadModuleOperation *o = new LoadModuleOperation("module-tunnel-sink", args, this);
    connect(o, SIGNAL(result(LoadModuleOperation*,uint32_t)), SLOT(tunnelCallback(LoadModuleOperation*,uint32_t)));
    o->exec(context);
}

void PAController::tunnelCallback(LoadModuleOperation *, uint32_t index)
{
    if (index == PA_INVALID_INDEX) {
        // ignore any errors, the module might be already loaded
        return;
    }

    tunnelModules << index;
}

void PAController::moveSinkInput(const SinkInput &input, const QList<QByteArray> &speakers)
{
    if (speakers.isEmpty())
        return;

    inputToBeMoved = input.index();

    if (speakers.count() == 1) {
        QByteArray sink = speakers.first().trimmed().prepend("tunnel.");
        MoveOperation *o = new MoveOperation(input, sink, this);
        connect(o, SIGNAL(result(MoveOperation*,bool)), SLOT(moveCallback(MoveOperation*,bool)));
        o->exec(context);
    } else {
        /* if > 1 speakers have been chosen, we have to combine all of them using combineTunnels() */
        combinedSinkName = "combined";
        foreach (const QByteArray &addr, speakers)
            combinedSinkName += addr.trimmed().prepend('_');

        combineTunnels(speakers, combinedSinkName, 10, "src-sinc-best-quality");
    }
}

void PAController::moveCallback(MoveOperation *o, bool success)
{
    if (!success)
        emit warning(tr("failed to move stream #%1 to sink %2")
                     .arg(o->input().index())
                     .arg(o->sink()));

    inputToBeMoved = PA_INVALID_INDEX;

    /* unload unused module-combine */
    if (oldCombineMod != PA_INVALID_INDEX) {
        UnloadModuleOperation *o = new UnloadModuleOperation(oldCombineMod, this);
        o->exec(context);
        oldCombineMod = PA_INVALID_INDEX;
    }
}

void PAController::combineTunnels(const QList<QByteArray> &addresses, const QString &name,
                                  int adjustTime, const QString &resampleMethod)
{
    /* build the list of arguments to module-combine */
    QStringList args;

    QString slaves;
    foreach (QByteArray addr, addresses)
        slaves += addr.trimmed().prepend("tunnel.") + ',';
    slaves.chop(1);
    args << slaves.prepend("slaves=");

    if (!name.trimmed().isEmpty())
        args << name.trimmed().prepend("sink_name=");
    if (adjustTime >= 0)
        args << QString::number(adjustTime).prepend("adjust_time=");
    if (!resampleMethod.trimmed().isEmpty())
        args << resampleMethod.trimmed().prepend("resample_method=");

    /* load module-combine */
    LoadModuleOperation *o = new LoadModuleOperation("module-combine", args, this);
    connect(o, SIGNAL(result(LoadModuleOperation*,uint32_t)), SLOT(combineCallback(LoadModuleOperation*,uint32_t)));
    o->exec(context);
}

void PAController::combineCallback(LoadModuleOperation *o, uint32_t index)
{
    if (index == PA_INVALID_INDEX) {
        emit warning(tr("failed to load %1")
                     .arg(o->name()));
        return;
    }

    oldCombineMod = combineMod;
    combineMod = index;

    if (inputToBeMoved != PA_INVALID_INDEX) {
        MoveOperation *o = new MoveOperation(SinkInput(inputToBeMoved), combinedSinkName, this);
        connect(o, SIGNAL(result(MoveOperation*,bool)), SLOT(moveCallback(MoveOperation*,bool)));
        o->exec(context);
    }
}
