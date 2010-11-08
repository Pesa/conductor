#include <limits>
#include <QMap>

#include "algorithm.h"

#define MAX_RETRIES 2
#define MAX_SIMULTANEOUS_SPEAKERS 3


Algorithm::Algorithm(QObject *parent) :
    QObject(parent),
    retryCount(0)
{
    // TODO: adjRooms = Config::getFoo();
}

void Algorithm::chooseOutputs()
{
    QHash<QString, QSet<QString> > newOutputs;

    QHashIterator<QString, QHash<QString,int> > i(rssiMap);
    while (i.hasNext()) {
        i.next();

        //updateCurrentRoom()??
        QString room = curRooms.value(i.key());
        if (room.isEmpty())
            continue;

        QSet<QString> results;
        QMap<int, QString> sorted;

        // sort rooms by RSSI
        QHashIterator<QString, int> j(i.value());
        while (j.hasNext()) {
            j.next();
            if (j.value() != InvalidRssi)
                sorted[j.value()] = j.key();
        }

        // choose the rooms with highest RSSI
        int n = 0;
        foreach (QString s, sorted) {
            if (n >= MAX_SIMULTANEOUS_SPEAKERS)
                break;
            results << s;
            ++n;
        }

        // wait a bit more if we don't have enough data
        if (results.isEmpty() && retryCount < MAX_RETRIES) {
            ++retryCount;
            continue;
        }
        retryCount = 0;

        // intersect chosen rooms with the rooms adjacent to the current one
        results &= adjRooms.value(room);

        if (results != curOutputs.value(room)) {
            curOutputs[room] = results;
            newOutputs[room] = results;
        }
    }

    if (!newOutputs.isEmpty())
        emit outputsChanged(newOutputs);
}

void Algorithm::setRssi(const QString &probe, const QString &device, int rssi)
{
    qDebug("%s [%s] => %i", qPrintable(probe), qPrintable(device), rssi); // FIXME: for testing only!
    rssiMap[device][probe] = rssi;
}

const int Algorithm::InvalidRssi = std::numeric_limits<int>::min();
