#ifndef PACONTROLLER_H
#define PACONTROLLER_H

#include <QList>
#include <QObject>

#include "sinkinputmodel.h"
#include "sinkmodel.h"

class MoveOperation;
struct pa_context;
struct pa_glib_mainloop;

class PAController : public QObject
{
    Q_OBJECT

public:
    explicit PAController(QObject *parent = 0);
    ~PAController();

    /*!
      Load a tunnel-sink module, creating a new sink that
      forwards its input to the other end of the tunnel.

      \param server The IP address of the host the tunnel will be connected to.
    */
    void createTunnel(const QByteArray &server);

    /*!
      Route the specified SinkInput to all the speakers in the list,
      i.e. to their respective sinks.

      \param input The SinkInput that must be moved.
      \param speakers List of IP addresses of the destination speakers.
    */
    void moveSinkInput(const SinkInput &input, const QList<QByteArray> &speakers);

    SinkInputModel *modelForSinkInputs() { return inputModel; }
    SinkModel *modelForSinks() { return sinkModel; }

public slots:
    /*!
      Connect to the specified server.
      If server is not specified or is empty, connect to the default
      server, usually the PulseAudio daemon running locally.

      \param server The IP address of the server to connect to.
    */
    void connectToDaemon(const QByteArray &server = QByteArray());

signals:
    void connected(const QString &server, bool local);
    void error(const QString &msg);
    void warning(const QString &msg);

private slots:
    void moveCallback(MoveOperation *o, bool success);

private:
    static void combineCallback(pa_context *c, uint32_t idx, void *userdata);
    static void stateCallback(pa_context *c, void *userdata);
    static void subscribeCallback(pa_context *c, pa_subscription_event_type_t t, uint32_t idx, void *userdata);
    static void tunnelCallback(pa_context *c, uint32_t idx, void *userdata);

    /*!
      Create a new virtual sink that combines all the specified tunnels
      into one. This allows to playback a single audio stream from all
      the combined speakers at the same time.

      \param addresses List of IP addresses of the speakers that must be combined.
      \param name The name for the new sink.
      \param adjustTime Time in seconds when to readjust the sample rate of all sinks.
      \param resampleMethod Resampling algorithm to use. See libsamplerate's documentation.
    */
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
