#include <QMap>

#include "algorithm.h"
#include "rssimodel.h"

#define MAX_RETRIES 2
#define MAX_SIMULTANEOUS_SPEAKERS 3


Algorithm::Algorithm(const RssiModel *model, QObject *parent) :
    QObject(parent),
    retryCount(0),
    rssi(model)
{
    // TODO: adjRooms = Config::getAdj();
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
        int n = 0;
        foreach (const QString &room, sorted) {
            if (n >= MAX_SIMULTANEOUS_SPEAKERS)
                break;
            results << room;
            ++n;
        }

        // wait a bit more if we don't have enough data
        if (results.isEmpty() && retryCount < MAX_RETRIES) {
            ++retryCount;
            continue;
        }
        retryCount = 0;

        QSet<QString> curRooms = curOutputs.value(device);
        if (!curRooms.isEmpty()) {
            // intersect chosen rooms with the rooms adjacent to the current ones
            QSet<QString> neighbors;
            foreach (const QString &room, curRooms)
                neighbors += adjRooms.value(room);
            results &= neighbors;
        }

        if (results != curRooms) {
            curOutputs[device] = results;
            newOutputs[device] = results;
        }
    }

    if (!newOutputs.isEmpty())
        emit outputsChanged(newOutputs);
}
