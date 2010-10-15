#ifndef PROBE_H
#define PROBE_H

#include <QObject>
#include <QVariantMap>

#include "bluezadapter.h"
#include "bluezmanager.h"

class Probe : public QObject
{
    Q_OBJECT

public:
    explicit Probe(QObject *parent = 0);

    bool isDiscovering() const { return discovering; }

public slots:
    bool startDiscovery();
    bool stopDiscovery();

private slots:
    void onDeviceFound(const QString &address, const QVariantMap &properties);

private:
    bool initAdapter();
    bool initManager();

    bool discovering;
    org::bluez::Adapter *adapter;
    org::bluez::Manager *manager;
};

#endif // PROBE_H
