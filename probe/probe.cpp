#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QTimer>
#include <QtDebug>

#include "probe.h"

#define BLUEZ_SERVICE           "org.bluez"
#define BLUEZ_ADAPTER_IFACE     "org.bluez.Adapter"
#define BLUEZ_MANAGER_IFACE     "org.bluez.Manager"


Probe::Probe(QObject *parent) :
    QObject(parent),
    discovering(false),
    adapter(0),
    manager(0)
{
    QTimer::singleShot(0, this, SLOT(startDiscovery()));
}

bool Probe::startDiscovery()
{
    if (!adapter && !initAdapter())
        return false;

    if (isDiscovering())
        return true;

    QDBusReply<void> reply = adapter->call("StartDiscovery");
    if (!reply.isValid()) {
        qCritical() << Q_FUNC_INFO << ": calling StartDiscovery() failed with" << reply.error().name();
        return false;
    }

    discovering = true;
    QTimer::singleShot(5000, this, SLOT(stopDiscovery())); // FIXME: remove
    return true;
}

bool Probe::stopDiscovery()
{
    if (!isDiscovering())
        return false;

    QDBusReply<void> reply = adapter->call("StopDiscovery");
    if (!reply.isValid()) {
        qCritical() << Q_FUNC_INFO << ": calling StopDiscovery() failed with" << reply.error().name();
        return false;
    }

    discovering = false;
    return true;
}

bool Probe::initManager()
{
    manager = new QDBusInterface(BLUEZ_SERVICE, "/", BLUEZ_MANAGER_IFACE,
                                 QDBusConnection::systemBus(), this);
    if (!manager->isValid()) {
        qCritical() << Q_FUNC_INFO << ": couldn't create" << BLUEZ_MANAGER_IFACE << "interface";
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

    QDBusReply<QDBusObjectPath> reply = manager->call("DefaultAdapter");
    if (!reply.isValid()) {
        qCritical() << Q_FUNC_INFO << ": calling DefaultAdapter() failed with" << reply.error().name();
        return false;
    }

    adapter = new QDBusInterface(BLUEZ_SERVICE, reply.value().path(), BLUEZ_ADAPTER_IFACE,
                                 QDBusConnection::systemBus(), this);
    if (!adapter->isValid()) {
        qCritical() << Q_FUNC_INFO << ": couldn't create" << BLUEZ_ADAPTER_IFACE << "interface";
        delete adapter;
        adapter = 0;
        return false;
    }

    return true;
}
