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

public slots:
    void connectToProbes();
    void disconnectFromProbes();
    void startMonitoring(const QString &device);
    void stopMonitoring(const QString &device);

signals:
    void ready();

private slots:
    void rssiChanged(const QString &probeName, const QString &device, int newRssi);

private:
    /* map: room name => probe IP address */
    QHash<QString, QString> addrs;

    /* map: room name => probe RPC interface */
    QHash<QString, ProbeInterface*> probes;

    /* map: device address => room name => RSSI */
    QHash<QString, QHash<QString,int> > rssi;

    /* group for probes' connected() signal */
    QxtSignalGroup *group;
};

#endif // PROBEMANAGER_H
