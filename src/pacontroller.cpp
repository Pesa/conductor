/**********************************************************************
 *
 * Copyright (C) 2011  Davide Pesavento
 *
 * This file is part of Conductor.
 *
 * Conductor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Conductor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Conductor.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************/

#include <QApplication>

#include <pulse/context.h>
#include <pulse/glib-mainloop.h>
#include <pulse/subscribe.h>

#include "pacontroller.h"
#include "paoperation.h"
#include "paoperationqueue.h"


PAController::PAController(QObject *parent) :
    QObject(parent),
    combineMod(PA_INVALID_INDEX),
    inputModel(new SinkInputModel(this)),
    sinkModel(new SinkModel(this))
{
    /* initialize PulseAudio data structures */
    mainloop = pa_glib_mainloop_new(NULL);
    context = pa_context_new(pa_glib_mainloop_get_api(mainloop), "conductor");

    connect(qApp, SIGNAL(lastWindowClosed()), SLOT(cleanup()));
}

PAController::~PAController()
{
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
        /* notify that we are now connected */
        emit self->connected(pa_context_get_server(c), pa_context_is_local(c));

        /* subscribe to client, sink and sink input events */
        pa_subscription_mask_t mask = (pa_subscription_mask_t) (PA_SUBSCRIPTION_MASK_CLIENT |
                                                                PA_SUBSCRIPTION_MASK_SINK |
                                                                PA_SUBSCRIPTION_MASK_SINK_INPUT);
        pa_context_set_subscribe_callback(c, PAController::subscribeCallback, self);
        SubscribeOperation *o = new SubscribeOperation(mask, self);
        o->exec(c);

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

    PAOperationQueue *q = new PAOperationQueue(this);

    /* figure out the target sink name */
    QByteArray sinkName;
    if (speakers.count() == 1)
        sinkName = speakers.first().trimmed().prepend("tunnel.");
    else {
        /* if > 1 speakers have been chosen, we have to combine all of them using module-combine */
        sinkName = "combined";
        foreach (const QByteArray &addr, speakers)
            sinkName += addr.trimmed().prepend('_');

        QStringList args = combineArguments(speakers, sinkName, 10, "src-sinc-best-quality");
        LoadModuleOperation *load = new LoadModuleOperation("module-combine", args);
        connect(load, SIGNAL(result(LoadModuleOperation*,uint32_t)), SLOT(combineCallback(LoadModuleOperation*,uint32_t)));
        q->enqueue(load);
    }

    /* move the stream */
    MoveOperation *move = new MoveOperation(input, sinkName);
    q->enqueue(move);

    if (combineMod != PA_INVALID_INDEX) {
        /* unload unused module-combine */
        UnloadModuleOperation *unload = new UnloadModuleOperation(combineMod);
        q->enqueue(unload);
    }

    q->exec(context);
}

void PAController::combineCallback(LoadModuleOperation *, uint32_t index)
{
    if (index != PA_INVALID_INDEX)
        combineMod = index;
}

QStringList PAController::combineArguments(const QList<QByteArray> &addresses, const QString &name,
                                           int adjustTime, const QString &resampleMethod)
{
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

    return args;
}

void PAController::cleanup()
{
    PAOperationQueue *q = new PAOperationQueue(this);
    q->setAbortOnFailure(false);

    /* unload all modules previously loaded by us */
    if (combineMod != PA_INVALID_INDEX)
        q->enqueue(new UnloadModuleOperation(combineMod));
    foreach (uint32_t idx, tunnelModules)
        q->enqueue(new UnloadModuleOperation(idx));

    connect(q, SIGNAL(error(int)), qApp, SLOT(quit()));
    connect(q, SIGNAL(finished()), qApp, SLOT(quit()));
    q->exec(context);
}
