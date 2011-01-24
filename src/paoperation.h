#ifndef PAOPERATION_H
#define PAOPERATION_H

#include <QObject>

#include <pulse/introspect.h>

#include "sinkinput.h"


class PAOperation : public QObject
{
    Q_OBJECT

public:
    explicit PAOperation(QObject *parent = 0);

public slots:
    virtual void exec(pa_context *c) = 0;

signals:
    void error(int errno);
    void finished();

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

public slots:
    void exec(pa_context *c);

protected:
    virtual pa_operation *getInfo(pa_context *c) = 0;

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
    pa_operation *getInfo(pa_context *c) { return pa_context_get_client_info(c, index(), callback, this); }

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
    pa_operation *getInfo(pa_context *c) { return pa_context_get_sink_info_by_index(c, index(), callback, this); }

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
    pa_operation *getInfo(pa_context *c) { return pa_context_get_sink_input_info(c, index(), callback, this); }

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

public slots:
    void exec(pa_context *c);

signals:
    void result(MoveOperation *op, bool success);

protected slots:
    void onError(int errno) const;

private:
    static void callback(pa_context *c, int success, void *userdata);

    SinkInput _input;
    QByteArray _sink;
};

#endif // PAOPERATION_H
