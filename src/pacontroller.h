#ifndef PACONTROLLER_H
#define PACONTROLLER_H

#include <QObject>
#include <QString>

#include "sinkinputmodel.h"
#include "sinkmodel.h"

struct pa_context;
struct pa_glib_mainloop;

class PAController : public QObject
{
    Q_OBJECT

public:
    ~PAController();

    static void destroy();
    static PAController *instance();

    static SinkInputModel *modelForSinkInputs() { return instance()->inputModel; }
    static SinkModel *modelForSinks() { return instance()->sinkModel; }

signals:
    void connected(const QString &server, bool local);
    void error(const QString &msg);
    void warning(const QString &msg);

private slots:
    void init();

private:
    explicit PAController(QObject *parent = 0);

    static void stateCallback(pa_context *c, void *);
    static void subscribeCallback(pa_context *c, pa_subscription_event_type_t t, uint32_t idx, void *);

    static PAController *_instance;
    pa_context *context;
    pa_glib_mainloop *mainloop;
    SinkInputModel *inputModel;
    SinkModel *sinkModel;
};

#endif // PACONTROLLER_H
