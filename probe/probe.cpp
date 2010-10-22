#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusReply>

#include "probe.h"

#define BLUEZ_SERVICE   "org.bluez"
#define MIN_RSSI_DELTA  4


Probe::Probe(QObject *parent) :
    QObject(parent),
    discovering(false),
    adapter(0),
    manager(0)
{
}

bool Probe::startDiscovery()
{
    if (!adapter && !initAdapter())
        return false;

    if (isDiscovering())
        return true;

    connect(adapter, SIGNAL(DeviceFound(QString, QVariantMap)), this, SLOT(onDeviceFound(QString, QVariantMap)));

    QDBusReply<void> reply = adapter->StartDiscovery();
    if (!reply.isValid()) {
        qCritical("StartDiscovery() failed with: %s", qPrintable(reply.error().name()));
        disconnect(adapter, SIGNAL(DeviceFound(QString, QVariantMap)), this, SLOT(onDeviceFound(QString, QVariantMap)));
        return false;
    }

    discovering = true;
    return true;
}

bool Probe::stopDiscovery()
{
    if (!isDiscovering())
        return true;

    QDBusReply<void> reply = adapter->StopDiscovery();
    if (!reply.isValid()) {
        qCritical("StopDiscovery() failed with: %s", qPrintable(reply.error().name()));
        return false;
    }

    disconnect(adapter, SIGNAL(DeviceFound(QString, QVariantMap)), this, SLOT(onDeviceFound(QString, QVariantMap)));
    discovering = false;
    return true;
}

void Probe::onDeviceFound(const QString &address, const QVariantMap &properties)
{
    //qDebug() << address << properties.value("Name").toString()
    //         << "=>" << properties.value("RSSI").toInt();
}

bool Probe::initManager()
{
    manager = new org::bluez::Manager(BLUEZ_SERVICE, "/", QDBusConnection::systemBus(), this);
    if (!manager->isValid()) {
        qCritical("Could not create %s interface!", qPrintable(manager->interface()));
        delete manager;
        manager = 0;
        return false;
    }

    return true;
}

bool Probe::initAdapter()
{
    if (!manager && !initManager())
        return false;

    QDBusReply<QDBusObjectPath> reply = manager->DefaultAdapter();
    if (!reply.isValid()) {
        qCritical("DefaultAdapter() failed with: %s", qPrintable(reply.error().name()));
        return false;
    }

    adapter = new org::bluez::Adapter(BLUEZ_SERVICE, reply.value().path(), QDBusConnection::systemBus(), this);
    if (!adapter->isValid()) {
        qCritical("Could not create %s interface!", qPrintable(adapter->interface()));
        delete adapter;
        adapter = 0;
        return false;
    }

    return true;
}
