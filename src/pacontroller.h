#ifndef PACONTROLLER_H
#define PACONTROLLER_H

#include <QObject>

#include "sinkinputmodel.h"
#include "sinkmodel.h"

struct pa_context;
struct pa_glib_mainloop;

class PAController : public QObject
{
    Q_OBJECT

public:
    explicit PAController(QObject *parent = 0);
    ~PAController();

    bool combineSinks(const QList<uint32_t> &sinks, const QString &name = QString());
    bool createTunnel(const QByteArray &server);

    SinkInputModel *modelForSinkInputs() { return inputModel; }
    SinkModel *modelForSinks() { return sinkModel; }

public slots:
    void connectToDaemon(const QByteArray &server = QByteArray());

signals:
    void connected(const QString &server, bool local);
    void error(const QString &msg);
    void warning(const QString &msg);

private:
    static void combineCallback(pa_context *c, uint32_t idx, void *userdata);
    static void stateCallback(pa_context *c, void *userdata);
    static void subscribeCallback(pa_context *c, pa_subscription_event_type_t t, uint32_t idx, void *userdata);
    static void tunnelCallback(pa_context *c, uint32_t idx, void *userdata);

    pa_context *context;
    pa_glib_mainloop *mainloop;
    SinkInputModel *inputModel;
    SinkModel *sinkModel;
};

#endif // PACONTROLLER_H
