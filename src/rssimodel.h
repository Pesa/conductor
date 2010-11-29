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
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QStringList devices() const { return _devices; }
    void addDevice(const QString &device);
    void removeDevice(const QString &device);

    int rssi(const QString &device, const QString &room) const;
    static const int InvalidRssi;

public slots:
    void setRssi(const QString &probe, const QString &device, int rssi);

private:
    /* addresses of devices */
    QStringList _devices;

    /* names of rooms */
    QStringList _rooms;

    /* map: device address => room name => RSSI */
    QHash<QString, QHash<QString,int> > rssiMap;
};

#endif // RSSIMODEL_H
