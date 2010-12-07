#include <limits>

#include "config.h"
#include "rssimodel.h"

RssiModel::RssiModel(QObject *parent) :
    QAbstractTableModel(parent),
    _rooms(Config::probesAddresses().uniqueKeys())
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

QVariant RssiModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        QString dev = _devices.at(index.row());
        QString room = _rooms.at(index.column());
        int val = rssi(dev, room);
        if (val == InvalidRssi)
            return QVariant("-");
        else
            return QVariant(val);
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
