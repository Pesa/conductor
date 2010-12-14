#include <QMap>

#include "algorithm.h"
#include "config.h"
#include "rssimodel.h"

Algorithm::Algorithm(const RssiModel *model, QObject *parent) :
    QObject(parent),
    retryCount(0),
    rssi(model),
    adjRooms(Config::roomsTopology())
{
}

void Algorithm::chooseOutputs()
{
    QHash<QString, QSet<QString> > newOutputs;

    foreach (const QString &device, rssi->devices()) {
        // sort rooms by RSSI
        QMap<int, QString> sorted;
        foreach (const QString &room, adjRooms.uniqueKeys()) {
            int r = rssi->rssi(device, room);
            if (r != RssiModel::InvalidRssi)
                sorted[r] = room;
        }

        // choose the rooms with highest RSSI
        QSet<QString> results;
        QMapIterator<int, QString> i(sorted);
        int n = 0;
        i.toBack();
        while (i.hasPrevious()) {
            i.previous();
            if (n >= Config::maxSimultaneousSpeakers())
                break;
            results << i.value();
            ++n;
        }

        // wait a bit more if we don't have enough data
        if (results.isEmpty() && retryCount < Config::maxRetries()) {
            ++retryCount;
            continue;
        }
        retryCount = 0;

        QSet<QString> curRooms = curOutputs.value(device);
        if (!curRooms.isEmpty()) {
            // intersect with rooms contiguous to the current ones
            QSet<QString> neighbors = curRooms;
            foreach (const QString &room, curRooms)
                neighbors.unite(adjRooms.value(room));
            results.intersect(neighbors);
        }

        if (results != curRooms) {
            curOutputs[device] = results;
            newOutputs[device] = results;
        }
    }

    if (!newOutputs.isEmpty())
        emit outputsChanged(newOutputs);
}
