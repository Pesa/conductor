#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QHash>
#include <QObject>
#include <QSet>

class Algorithm : public QObject
{
    Q_OBJECT

public:
    explicit Algorithm(QObject *parent = 0);

public slots:
    void chooseOutputs();
    void setRssi(const QString &probe, const QString &device, int rssi);

signals:
    void outputsChanged(const QHash<QString, QSet<QString> > &outputs);

private:
    static const int InvalidRssi;
    int retryCount;

    /* map: room name => adjacent rooms */
    const QHash<QString, QSet<QString> > adjRooms;

    /* map: device address => current output rooms */
    QHash<QString, QSet<QString> > curOutputs;

    /* map: device address => current room */
    QHash<QString, QString> curRooms;

    /* map: device address => room name => RSSI */
    QHash<QString, QHash<QString,int> > rssiMap;
};

#endif // ALGORITHM_H
