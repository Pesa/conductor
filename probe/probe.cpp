/**********************************************************************
 *
 * Copyright (C) 2011  Davide Pesavento
 *
 * This file is part of Conductor.
 *
 * Conductor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Conductor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Conductor.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************/

#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QxtRPCPeer>

#include "probe.h"

#define BLUEZ_SERVICE   "org.bluez"
#define MIN_RSSI_DELTA  5
#define RPC_PORT        1234


Probe::Probe(QObject *parent) :
    QObject(parent),
    adapter(0),
    manager(0),
    rpc(new QxtRPCPeer(this)),
    discovering(false)
{
    rpc->attachSlot("hello", this, SLOT(hello(quint64,QString)));
    rpc->attachSlot("addDevice", this, SLOT(addDevice(quint64,QString)));
    rpc->attachSlot("removeDevice", this, SLOT(removeDevice(quint64,QString)));
    rpc->attachSlot("startDiscovery", this, SLOT(startDiscovery(quint64)));
    rpc->attachSlot("stopDiscovery", this, SLOT(stopDiscovery(quint64)));
    rpc->attachSignal(this, SIGNAL(rssiChanged(QString,QString,int)), "rssiChanged");

    rpc->listen(QHostAddress::Any, RPC_PORT);
}

void Probe::hello(quint64, const QString &probeName)
{
    myName = probeName;
}

void Probe::addDevice(quint64, const QString &address)
{
    devices.insert(address);
}

void Probe::removeDevice(quint64, const QString &address)
{
    devices.remove(address);
}

bool Probe::startDiscovery(quint64)
{
    if (!adapter && !initAdapter())
        return false;

    if (isDiscovering())
        return true;

    connect(adapter, SIGNAL(DeviceFound(QString,QVariantMap)), SLOT(onDeviceFound(QString,QVariantMap)));

    QDBusReply<void> reply = adapter->StartDiscovery();
    if (!reply.isValid()) {
        qCritical("StartDiscovery() failed with: %s", qPrintable(reply.error().name()));
        adapter->disconnect(SIGNAL(DeviceFound(QString,QVariantMap)));
        return false;
    }

    discovering = true;
    return true;
}

bool Probe::stopDiscovery(quint64)
{
    if (!isDiscovering())
        return true;

    QDBusReply<void> reply = adapter->StopDiscovery();
    if (!reply.isValid()) {
        qCritical("StopDiscovery() failed with: %s", qPrintable(reply.error().name()));
        return false;
    }

    adapter->disconnect(SIGNAL(DeviceFound(QString,QVariantMap)));
    discovering = false;
    return true;
}

void Probe::onDeviceFound(const QString &address, const QVariantMap &properties)
{
    if (!devices.contains(address))
        return;

    int newRssi = properties.value("RSSI").toInt();
    if (!rssi.contains(address) || (qAbs(rssi.value(address) - newRssi) >= MIN_RSSI_DELTA)) {
        rssi[address] = newRssi;
        emit rssiChanged(myName, address, newRssi);
    }
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
