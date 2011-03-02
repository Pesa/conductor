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
    void hello(quint64, const QString &probeName);
    void addDevice(quint64, const QString &address);
    void removeDevice(quint64, const QString &address);
    bool startDiscovery(quint64 = 0);
    bool stopDiscovery(quint64 = 0);

signals:
    void rssiChanged(const QString &probeName, const QString &device, int newRssi);

private slots:
    void onDeviceFound(const QString &address, const QVariantMap &properties);

private:
    bool initAdapter();
    bool initManager();

    org::bluez::Adapter *adapter;
    org::bluez::Manager *manager;
    QxtRPCPeer *rpc;

    bool discovering;
    QString myName;
    QSet<QString> devices;
    QHash<QString, int> rssi;
};

#endif // PROBE_H
