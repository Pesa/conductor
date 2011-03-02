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

#ifndef PROBEINTERFACE_H
#define PROBEINTERFACE_H

#include <QxtRPCPeer>

class ProbeInterface : public QxtRPCPeer
{
    Q_OBJECT

public:
    ProbeInterface(const QString &name, const QString &address, QObject *parent = 0);

public slots:
    void connect();
    void disconnect();
    void addDevice(const QString &address);
    void removeDevice(const QString &address);
    void startDiscovery();
    void stopDiscovery();

signals:
    void connected(const QString &name);
    void disconnected(const QString &name);
    void error(const QString &name);
    void rssiChanged(const QString &name, const QString &device, int newRssi);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError errorCode);

private:
    QString _name;
    QString _addr;
};

#endif // PROBEINTERFACE_H
