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

    QByteArray addressOfProbe(const QString &probe) const { return addrs.value(probe).toLocal8Bit(); }

public slots:
    void connectToProbes();
    void disconnectFromProbes();
    void startMonitoring(const QString &device);
    void stopMonitoring(const QString &device);

signals:
    void ready();
    void rssiChanged(const QString &probe, const QString &device, int rssi);

private:
    /* map: room name => probe IP address */
    QHash<QString, QString> addrs;

    /* map: room name => probe RPC interface */
    QHash<QString, ProbeInterface*> probes;

    /* group for probes' connected() signal */
    QxtSignalGroup *group;
};

#endif // PROBEMANAGER_H
