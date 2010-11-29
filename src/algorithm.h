#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QHash>
#include <QObject>
#include <QSet>

class RssiModel;

class Algorithm : public QObject
{
    Q_OBJECT

public:
    explicit Algorithm(const RssiModel *model, QObject *parent = 0);

public slots:
    void chooseOutputs();

signals:
    void outputsChanged(const QHash<QString, QSet<QString> > &outputs);

private:
    int retryCount;
    const RssiModel *rssi;

    /* map: room name => adjacent rooms */
    const QHash<QString, QSet<QString> > adjRooms;

    /* map: device address => current output rooms */
    QHash<QString, QSet<QString> > curOutputs;

    /* map: device address => current room */
    QHash<QString, QString> curRooms;
};

#endif // ALGORITHM_H
