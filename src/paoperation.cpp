#include <pulse/error.h>
#include <pulse/operation.h>

#include "paoperation.h"


PAOperation::PAOperation(QObject *parent) :
    QObject(parent)
{
    connect(this, SIGNAL(error(int)), SLOT(onError(int)));
}

void PAOperation::onError(int errno) const
{
    qWarning("%s failed: %s",
             sender()->metaObject()->className(),
             pa_strerror(errno));
}

void GetInfoOperation::exec(pa_context *c)
{
    pa_operation *op = getInfo(c);
    if (op)
        pa_operation_unref(op);
    else
        emit error(pa_context_errno(c));
}

void GetInfoOperation::onError(int errno) const
{
    qWarning("%s (index #%u) failed: %s",
             sender()->metaObject()->className(),
             _index, pa_strerror(errno));
}

void ClientInfoOperation::callback(pa_context *c, const pa_client_info *i, int eol, void *userdata)
{
    ClientInfoOperation *op = static_cast<ClientInfoOperation*>(userdata);

    if (eol == 0)
        emit op->result(op, i);
    else if (eol > 0)
        emit op->finished();
    else
        emit op->error(pa_context_errno(c));

    op->deleteLater();
}

void SinkInfoOperation::callback(pa_context *c, const pa_sink_info *i, int eol, void *userdata)
{
    SinkInfoOperation *op = static_cast<SinkInfoOperation*>(userdata);

    if (eol == 0)
        emit op->result(op, i);
    else if (eol > 0)
        emit op->finished();
    else
        emit op->error(pa_context_errno(c));

    op->deleteLater();
}

void SinkInputInfoOperation::callback(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata)
{
    SinkInputInfoOperation *op = static_cast<SinkInputInfoOperation*>(userdata);

    if (eol == 0)
        emit op->result(op, i);
    else if (eol > 0)
        emit op->finished();
    else
        emit op->error(pa_context_errno(c));

    op->deleteLater();
}

void MoveOperation::exec(pa_context *c)
{
    pa_operation *op = pa_context_move_sink_input_by_name(c, _input.index(), _sink.constData(), callback, this);
    if (op)
        pa_operation_unref(op);
    else
        emit error(pa_context_errno(c));
}

void MoveOperation::onError(int errno) const
{
    qWarning("%s of sink input #%u to %s failed: %s",
             sender()->metaObject()->className(),
             _input.index(), _sink.constData(),
             pa_strerror(errno));
}

void MoveOperation::callback(pa_context *c, int success, void *userdata)
{
    MoveOperation *op = static_cast<MoveOperation*>(userdata);

    emit op->result(op, success);

    if (success)
        emit op->finished();
    else
        emit op->error(pa_context_errno(c));

    op->deleteLater();
}

void LoadModuleOperation::exec(pa_context *c)
{
    QByteArray args = _args.join(" ").toLocal8Bit();
    pa_operation *op = pa_context_load_module(c, _name.constData(), args.constData(), callback, this);
    if (op)
        pa_operation_unref(op);
    else
        emit error(pa_context_errno(c));
}

void LoadModuleOperation::onError(int errno) const
{
    qWarning("%s (%s) failed: %s",
             sender()->metaObject()->className(),
             _name.constData(), pa_strerror(errno));
}

void LoadModuleOperation::callback(pa_context *c, uint32_t idx, void *userdata)
{
    LoadModuleOperation *op = static_cast<LoadModuleOperation*>(userdata);

    emit op->result(op, idx);

    if (idx == PA_INVALID_INDEX)
        emit op->error(pa_context_errno(c));
    else
        emit op->finished();

    op->deleteLater();
}

void UnloadModuleOperation::exec(pa_context *c)
{
    pa_operation *op = pa_context_unload_module(c, _index, callback, this);
    if (op)
        pa_operation_unref(op);
    else
        emit error(pa_context_errno(c));
}

void UnloadModuleOperation::callback(pa_context *c, int success, void *userdata)
{
    UnloadModuleOperation *op = static_cast<UnloadModuleOperation*>(userdata);

    emit op->result(op, success);

    if (success)
        emit op->finished();
    else
        emit op->error(pa_context_errno(c));

    op->deleteLater();
}
