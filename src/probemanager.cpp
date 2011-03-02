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

#include <QxtSignalGroup>

#include "config.h"
#include "probemanager.h"
#include "probeinterface.h"

ProbeManager::ProbeManager(QObject *parent) :
    QObject(parent),
    addrs(Config::probesAddresses()),
    group(new QxtSignalGroup(this))
{
    QHashIterator<QString, QByteArray> i(addrs);
    while (i.hasNext()) {
        i.next();
        ProbeInterface *probe = new ProbeInterface(i.key(), i.value());
        probes[i.key()] = probe;
        group->addSignal(probe, SIGNAL(connected(QString)));
        connect(probe, SIGNAL(rssiChanged(QString,QString,int)), SIGNAL(rssiChanged(QString,QString,int)));
    }

    connect(group, SIGNAL(allSignalsReceived()), SIGNAL(ready()));
}

ProbeManager::~ProbeManager()
{
    qDeleteAll(probes);
    probes.clear();
}

void ProbeManager::connectToProbes()
{
    group->reset();
    foreach (ProbeInterface *i, probes)
        i->connect();
}

void ProbeManager::disconnectFromProbes()
{
    foreach (ProbeInterface *i, probes)
        i->disconnect();
}

void ProbeManager::startMonitoring(const QString &device)
{
    foreach (ProbeInterface *i, probes) {
        i->addDevice(device);
        i->startDiscovery();
    }
}

void ProbeManager::stopMonitoring(const QString &device)
{
    foreach (ProbeInterface *i, probes) {
        i->stopDiscovery();
        i->removeDevice(device);
    }
}
