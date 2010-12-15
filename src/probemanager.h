#ifndef PROBEMANAGER_H
#define PROBEMANAGER_H

#include <QHash>
#include <QObject>

class QxtSignalGroup;
class ProbeInterface;

class ProbeManager : public QObject
{
    Q_OBJECT

public:
    explicit ProbeManager(QObject *parent = 0);
    ~ProbeManager();

    QByteArray addressOfProbe(const QString &probe) const { return addrs.value(probe); }

public slots:
    void connectToProbes();
    void disconnectFromProbes();
    void startMonitoring(const QString &device);
    void stopMonitoring(const QString &device);

signals:
    /*!
      Emitted when all configured probes are connected
      to ProbeManager and have been fully initialized.
    */
    void ready();

    /*!
      Notifies the change of RSSI value for the specified probe and device.
    */
    void rssiChanged(const QString &probe, const QString &device, int rssi);

private:
    /* map: room name => probe IP address */
    QHash<QString, QByteArray> addrs;

    /* map: room name => probe RPC interface */
    QHash<QString, ProbeInterface*> probes;

    /* group for probes' connected() signal */
    QxtSignalGroup *group;
};

#endif // PROBEMANAGER_H
