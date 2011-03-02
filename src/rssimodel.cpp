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

#include <limits>

#include "config.h"
#include "rssimodel.h"

RssiModel::RssiModel(QObject *parent) :
    QAbstractTableModel(parent),
    _editable(false),
    _rooms(Config::roomsNames())
{
}

int RssiModel::columnCount(const QModelIndex &) const
{
    return _rooms.count();
}

int RssiModel::rowCount(const QModelIndex &) const
{
    return _devices.count();
}

Qt::ItemFlags RssiModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags itemFlags = QAbstractTableModel::flags(index);
    if (_editable)
        itemFlags |= Qt::ItemIsEditable;

    return itemFlags;
}

QVariant RssiModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QString dev = _devices.at(index.row());
        QString room = _rooms.at(index.column());
        int val = rssi(dev, room);
        if (val == InvalidRssi)
            return QString("-");
        else
            return QString::number(val);
    } else if (role == Qt::TextAlignmentRole) {
        return QVariant(Qt::AlignCenter);
    }

    return QVariant();
}

QVariant RssiModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal && section < _rooms.size())
            return _rooms.at(section);
        else if (orientation == Qt::Vertical && section < _devices.size())
            return _devices.at(section);
    }

    return QVariant();
}

bool RssiModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int i;
        bool ok = false;
        if (value.toString().isEmpty() || value.toString() == "-") {
            i = InvalidRssi;
            ok = true;
        } else {
            i = value.toInt(&ok);
        }

        if (ok) {
            rssiMap[_devices.at(index.row())][_rooms.at(index.column())] = i;
            emit dataChanged(index, index);
            return true;
        }
    }

    return false;
}

void RssiModel::addDevice(const QString &device)
{
    if (!_devices.contains(device)) {
        beginInsertRows(QModelIndex(), _devices.count(), _devices.count());
        _devices.append(device);
        endInsertRows();
    }
}

void RssiModel::removeDevice(const QString &device)
{
    int i = _devices.indexOf(device);
    if (i != -1) {
        beginRemoveRows(QModelIndex(), i, i);
        _devices.removeAt(i);
        rssiMap.remove(device);
        endRemoveRows();
    }
}

int RssiModel::rssi(const QString &device, const QString &room) const
{
    return rssiMap.value(device).value(room, InvalidRssi);
}

void RssiModel::setRssi(const QString &probe, const QString &device, int rssi)
{
    rssiMap[device][probe] = rssi;

    int row = _devices.indexOf(device);
    int col = _rooms.indexOf(probe);
    emit dataChanged(createIndex(row, col), createIndex(row, col));
}

const int RssiModel::InvalidRssi = std::numeric_limits<int>::min();
