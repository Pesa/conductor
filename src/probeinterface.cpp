#include "probeinterface.h"

#define RPC_PORT 1234

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
    QxtRPCPeer::connect(_addr, RPC_PORT);
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
    qWarning("ProbeInterface '%s': socket error %i", qPrintable(_name), errorCode);
    emit error(_name);
}
