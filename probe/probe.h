#ifndef PROBE_H
#define PROBE_H

#include <QObject>
#include <QHash>
#include <QSet>
#include <QVariantMap>
#include <QxtRPCPeer>

#include "bluezadapter.h"
#include "bluezmanager.h"

class Probe : public QObject
{
    Q_OBJECT

public:
    explicit Probe(QObject *parent = 0);

    bool isDiscovering() const { return discovering; }

public slots:
    void addDevice(quint64 id, const QString &address);
    void removeDevice(quint64 id, const QString &address);
    bool startDiscovery(quint64 id = 0);
    bool stopDiscovery(quint64 id = 0);

signals:
    void rssiChanged(const QString &device, int newRssi);

private slots:
    void onDeviceFound(const QString &address, const QVariantMap &properties);

private:
    bool initAdapter();
    bool initManager();

    org::bluez::Adapter *adapter;
    org::bluez::Manager *manager;
    QxtRPCPeer *rpc;

    bool discovering;
    QSet<QString> devices;
    QHash<QString, int> rssi;
};

#endif // PROBE_H
