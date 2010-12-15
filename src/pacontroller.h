#ifndef PACONTROLLER_H
#define PACONTROLLER_H

#include <QList>
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

	//create a sink that acts as tunnel connection with the server
    void createTunnel(const QByteArray &server);
    //move the audio stream identified by the SinkInput towards the speakers detected by the algorithm  
    void moveSinkInput(const SinkInput &input, const QList<QByteArray> &speakers);

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
    static void moveCallback(pa_context *c, int success, void *userdata);
    static void stateCallback(pa_context *c, void *userdata);
    static void subscribeCallback(pa_context *c, pa_subscription_event_type_t t, uint32_t idx, void *userdata);
    static void tunnelCallback(pa_context *c, uint32_t idx, void *userdata);
	//create a new sink that combines sinks contained in the first parameter for a simultaneous playback
    void combineTunnels(const QList<QByteArray> &addresses, const QString &name = QString(),
                        int adjustTime = -1, const QString &resampleMethod = QString());

    pa_context *context;
    pa_glib_mainloop *mainloop;
    QByteArray combinedSinkName;
    uint32_t combineMod;
    uint32_t oldCombineMod;
    uint32_t inputToBeMoved;
    SinkInputModel *inputModel;
    SinkModel *sinkModel;
    QList<uint32_t> tunnelModules;
};

#endif // PACONTROLLER_H
