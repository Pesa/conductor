#include <QTimer>

#include <pulse/context.h>
#include <pulse/glib-mainloop.h>
#include <pulse/operation.h>
#include <pulse/subscribe.h>

#include "pacontroller.h"


PAController::PAController(QObject *parent) :
    QObject(parent),
    context(0),
    mainloop(0),
    inputModel(new SinkInputModel(this)),
    sinkModel(new SinkModel(this))
{
    QTimer::singleShot(0, this, SLOT(init()));
}

PAController::~PAController()
{
    if (context) {
        pa_context_disconnect(context);
        pa_context_unref(context);
        context = 0;
    }

    if (mainloop) {
        pa_glib_mainloop_free(mainloop);
        mainloop = 0;
    }

    _instance = 0;
}

void PAController::destroy()
{
    if (_instance)
        delete _instance;
}

PAController *PAController::instance()
{
    if (!_instance)
        _instance = new PAController;

    return _instance;
}

void PAController::init()
{
    /* initialize PulseAudio data structures */
    mainloop = pa_glib_mainloop_new(NULL);
    context = pa_context_new(pa_glib_mainloop_get_api(mainloop), "conductor");

    pa_context_set_state_callback(context, PAController::stateCallback, NULL);

    /* connect to the daemon */
    if (pa_context_connect(context, NULL, PA_CONTEXT_NOFAIL, NULL) < 0)
        emit error(tr("pa_context_connect() failed"));
}

void PAController::stateCallback(pa_context *c, void *)
{
    switch (pa_context_get_state(c)) {

    case PA_CONTEXT_READY: {
        pa_operation *o;

        emit instance()->connected(pa_context_get_server(c), pa_context_is_local(c));

        /* subscribe to client, sink and sink input events */
        pa_context_set_subscribe_callback(c, PAController::subscribeCallback, NULL);
        if (!(o = pa_context_subscribe(c, (pa_subscription_mask_t)
                                       (PA_SUBSCRIPTION_MASK_CLIENT |
                                        PA_SUBSCRIPTION_MASK_SINK |
                                        PA_SUBSCRIPTION_MASK_SINK_INPUT),
                                       NULL, NULL)))
            emit instance()->warning(tr("pa_context_subscribe() failed"));
        else
            pa_operation_unref(o);

        /* initialize the table models */
        if (!instance()->inputModel->populate(c))
            emit instance()->warning(tr("failed to populate SinkInputModel"));
        if (!instance()->sinkModel->populate(c))
            emit instance()->warning(tr("failed to populate SinkModel"));

        break;
    }

    case PA_CONTEXT_FAILED:
        emit instance()->error(tr("PA context failure"));
        break;

    default:
        break;
    }
}

void PAController::subscribeCallback(pa_context *c, pa_subscription_event_type_t t, uint32_t idx, void *)
{
    /* dispatch the event to the appropriate model for further processing */
    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {

    case PA_SUBSCRIPTION_EVENT_CLIENT:
        switch (t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) {
        case PA_SUBSCRIPTION_EVENT_NEW:
        case PA_SUBSCRIPTION_EVENT_CHANGE:
            instance()->inputModel->updateClient(c, idx);
            break;
        case PA_SUBSCRIPTION_EVENT_REMOVE:
            instance()->inputModel->removeClient(idx);
            break;
        }
        break;

    case PA_SUBSCRIPTION_EVENT_SINK:
        switch (t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) {
        case PA_SUBSCRIPTION_EVENT_NEW:
        case PA_SUBSCRIPTION_EVENT_CHANGE:
            instance()->sinkModel->updateSink(c, idx);
            break;
        case PA_SUBSCRIPTION_EVENT_REMOVE:
            instance()->sinkModel->removeSink(idx);
            break;
        }
        break;

    case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
        switch (t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) {
        case PA_SUBSCRIPTION_EVENT_NEW:
        case PA_SUBSCRIPTION_EVENT_CHANGE:
            instance()->inputModel->updateSinkInput(c, idx);
            break;
        case PA_SUBSCRIPTION_EVENT_REMOVE:
            instance()->inputModel->removeSinkInput(idx);
            break;
        }
        break;
    }
}

PAController *PAController::_instance = 0;
