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

#include "probeinterface.h"
#include "config.h"

ProbeInterface::ProbeInterface(const QString &name, const QString &address, QObject *parent) :
    QxtRPCPeer(parent),
    _name(name),
    _addr(address)
{
    QObject::connect(this, SIGNAL(connectedToServer()), SLOT(onConnected()));
    QObject::connect(this, SIGNAL(disconnectedFromServer()), SLOT(onDisconnected()));
    QObject::connect(this, SIGNAL(serverError(QAbstractSocket::SocketError)), SLOT(onError(QAbstractSocket::SocketError)));
    attachSlot("rssiChanged", this, SIGNAL(rssiChanged(QString,QString,int)));
}

void ProbeInterface::connect()
{
    QxtRPCPeer::connect(_addr, Config::rpcPort());
}

void ProbeInterface::disconnect()
{
    QxtRPCPeer::disconnectAll();
}

void ProbeInterface::addDevice(const QString &address)
{
    call("addDevice", address);
}

void ProbeInterface::removeDevice(const QString &address)
{
    call("removeDevice", address);
}

void ProbeInterface::startDiscovery()
{
    call("startDiscovery");
}

void ProbeInterface::stopDiscovery()
{
    call("stopDiscovery");
}

void ProbeInterface::onConnected()
{
    call("hello", _name);
    emit connected(_name);
}

void ProbeInterface::onDisconnected()
{
    emit disconnected(_name);
}

void ProbeInterface::onError(QAbstractSocket::SocketError errorCode)
{
    qCritical("ProbeInterface '%s': socket error %i", qPrintable(_name), errorCode);
    emit error(_name);
}
