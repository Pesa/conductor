#ifndef PAOPERATION_H
#define PAOPERATION_H

#include <QByteArray>
#include <QObject>
#include <QStringList>

#include <pulse/introspect.h>

#include "sinkinput.h"


class PAOperation : public QObject
{
    Q_OBJECT

public:
    explicit PAOperation(QObject *parent = 0);

public slots:
    void exec(pa_context *c);

signals:
    void error(int errno);
    void finished();

protected:
    virtual pa_operation *execImpl(pa_context *c) = 0;

protected slots:
    virtual void onError(int errno) const;
};


class GetInfoOperation : public PAOperation
{
    Q_OBJECT

public:
    explicit GetInfoOperation(uint32_t index, QObject *parent = 0)
        : PAOperation(parent), _index(index) {}

    uint32_t index() const { return _index; }

protected slots:
    void onError(int errno) const;

private:
    uint32_t _index;
};


class ClientInfoOperation : public GetInfoOperation
{
    Q_OBJECT

public:
    explicit ClientInfoOperation(uint32_t index, QObject *parent = 0)
        : GetInfoOperation(index, parent) {}

signals:
    void result(ClientInfoOperation *op, const pa_client_info *info);

protected:
    pa_operation *execImpl(pa_context *c);

private:
    static void callback(pa_context *c, const pa_client_info *i, int eol, void *userdata);
};


class SinkInfoOperation : public GetInfoOperation
{
    Q_OBJECT

public:
    explicit SinkInfoOperation(uint32_t index, QObject *parent = 0)
        : GetInfoOperation(index, parent) {}

signals:
    void result(SinkInfoOperation *op, const pa_sink_info *info);

protected:
    pa_operation *execImpl(pa_context *c);

private:
    static void callback(pa_context *c, const pa_sink_info *i, int eol, void *userdata);
};


class SinkInputInfoOperation : public GetInfoOperation
{
    Q_OBJECT

public:
    explicit SinkInputInfoOperation(uint32_t index, QObject *parent = 0)
        : GetInfoOperation(index, parent) {}

signals:
    void result(SinkInputInfoOperation *op, const pa_sink_input_info *info);

protected:
    pa_operation *execImpl(pa_context *c);

private:
    static void callback(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata);
};


class MoveOperation : public PAOperation
{
    Q_OBJECT

public:
    MoveOperation(const SinkInput &input, const QByteArray &sinkName, QObject *parent = 0)
        : PAOperation(parent), _input(input), _sink(sinkName) {}

    SinkInput input() const { return _input; }
    QString sink() const { return _sink; }

protected:
    pa_operation *execImpl(pa_context *c);

protected slots:
    void onError(int errno) const;

private:
    static void callback(pa_context *c, int success, void *userdata);

    SinkInput _input;
    QByteArray _sink;
};


class LoadModuleOperation : public PAOperation
{
    Q_OBJECT

public:
    LoadModuleOperation(const QByteArray &moduleName, const QStringList &args, QObject *parent = 0)
        : PAOperation(parent), _name(moduleName), _args(args) {}

    QString name() const { return _name; }

signals:
    void result(LoadModuleOperation *op, uint32_t index);

protected:
    pa_operation *execImpl(pa_context *c);

protected slots:
    void onError(int errno) const;

private:
    static void callback(pa_context *c, uint32_t idx, void *userdata);

    QByteArray _name;
    QStringList _args;
};


class UnloadModuleOperation : public PAOperation
{
    Q_OBJECT

public:
    explicit UnloadModuleOperation(uint32_t index, QObject *parent = 0)
        : PAOperation(parent), _index(index) {}

    uint32_t index() const { return _index; }

protected:
    pa_operation *execImpl(pa_context *c);

private:
    static void callback(pa_context *c, int success, void *userdata);

    uint32_t _index;
};


class SubscribeOperation : public PAOperation
{
    Q_OBJECT

public:
    explicit SubscribeOperation(pa_subscription_mask_t mask, QObject *parent = 0)
        : PAOperation(parent), _mask(mask) {}

protected:
    pa_operation *execImpl(pa_context *c);

private:
    static void callback(pa_context *c, int success, void *userdata);

    pa_subscription_mask_t _mask;
};

#endif // PAOPERATION_H
