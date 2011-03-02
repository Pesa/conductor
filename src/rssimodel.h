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

#ifndef RSSIMODEL_H
#define RSSIMODEL_H

#include <QAbstractTableModel>
#include <QHash>
#include <QStringList>

class RssiModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit RssiModel(QObject *parent = 0);

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QStringList devices() const { return _devices; }
    void addDevice(const QString &device);
    void removeDevice(const QString &device);

    int rssi(const QString &device, const QString &room) const;
    static const int InvalidRssi;

public slots:
    void setEditable(bool editable) { _editable = editable; }
    void setRssi(const QString &probe, const QString &device, int rssi);

private:
    /* is the model editable? */
    bool _editable;

    /* addresses of devices */
    QStringList _devices;

    /* names of rooms */
    QStringList _rooms;

    /* map: device address => room name => RSSI */
    QHash<QString, QHash<QString,int> > rssiMap;
};

#endif // RSSIMODEL_H
