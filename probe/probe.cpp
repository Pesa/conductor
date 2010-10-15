#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QTimer>
#include <QtDebug>

#include "probe.h"

#define BLUEZ_SERVICE   "org.bluez"


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

    connect(adapter, SIGNAL(DeviceFound(QString, QVariantMap)), this, SLOT(onDeviceFound(QString, QVariantMap)));

    QDBusReply<void> reply = adapter->StartDiscovery();
    if (!reply.isValid()) {
        qCritical() << Q_FUNC_INFO << ": calling StartDiscovery() failed with" << reply.error().name();
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
        qCritical() << Q_FUNC_INFO << ": calling StopDiscovery() failed with" << reply.error().name();
        return false;
    }

    disconnect(adapter, SIGNAL(DeviceFound(QString, QVariantMap)), this, SLOT(onDeviceFound(QString, QVariantMap)));
    discovering = false;
    return true;
}

void Probe::onDeviceFound(const QString &address, const QVariantMap &properties)
{
    qDebug() << address << properties.value("Name").toString()
             << "=>" << properties.value("RSSI").toInt();
}

bool Probe::initManager()
{
    manager = new org::bluez::Manager(BLUEZ_SERVICE, "/", QDBusConnection::systemBus(), this);
    if (!manager->isValid()) {
        qCritical() << Q_FUNC_INFO << ": couldn't create" << manager->interface() << "interface";
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
        qCritical() << Q_FUNC_INFO << ": calling DefaultAdapter() failed with" << reply.error().name();
        return false;
    }

    adapter = new org::bluez::Adapter(BLUEZ_SERVICE, reply.value().path(), QDBusConnection::systemBus(), this);
    if (!adapter->isValid()) {
        qCritical() << Q_FUNC_INFO << ": couldn't create" << adapter->interface() << "interface";
        delete adapter;
        adapter = 0;
        return false;
    }

    return true;
}
