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

#ifndef PROBEMANAGER_H
#define PROBEMANAGER_H

#include <QHash>
#include <QObject>

class QxtSignalGroup;
class ProbeInterface;

class ProbeManager : public QObject
{
    Q_OBJECT

public:
    explicit ProbeManager(QObject *parent = 0);
    ~ProbeManager();

    QByteArray addressOfProbe(const QString &probe) const { return addrs.value(probe); }

public slots:
    void connectToProbes();
    void disconnectFromProbes();
    void startMonitoring(const QString &device);
    void stopMonitoring(const QString &device);

signals:
    /*!
      Emitted when all configured probes are connected
      to ProbeManager and have been fully initialized.
    */
    void ready();

    /*!
      Notifies the change of RSSI value for the specified probe and device.
    */
    void rssiChanged(const QString &probe, const QString &device, int rssi);

private:
    /* map: room name => probe IP address */
    QHash<QString, QByteArray> addrs;

    /* map: room name => probe RPC interface */
    QHash<QString, ProbeInterface*> probes;

    /* group for probes' connected() signal */
    QxtSignalGroup *group;
};

#endif // PROBEMANAGER_H
