#ifndef PROBE_H
#define PROBE_H

#include <QObject>
#include <QHash>
#include <QSet>

#include "bluezadapter.h"
#include "bluezmanager.h"

class QxtRPCPeer;

class Probe : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool discovering READ isDiscovering)

public:
    explicit Probe(QObject *parent = 0);

    bool isDiscovering() const { return discovering; }

public slots:
    void hello(quint64, qulonglong probeId);
    void addDevice(quint64, const QString &address);
    void removeDevice(quint64, const QString &address);
    bool startDiscovery(quint64 = 0);
    bool stopDiscovery(quint64 = 0);

signals:
    void rssiChanged(qulonglong probeId, const QString &device, int newRssi);

private slots:
    void onDeviceFound(const QString &address, const QVariantMap &properties);

private:
    bool initAdapter();
    bool initManager();

    org::bluez::Adapter *adapter;
    org::bluez::Manager *manager;
    QxtRPCPeer *rpc;

    bool discovering;
    qulonglong myId;
    QSet<QString> devices;
    QHash<QString, int> rssi;
};

#endif // PROBE_H
