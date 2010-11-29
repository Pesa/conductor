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

    foreach (QString device, rssi->devices()) {
        //updateCurrentRoom()??
        QString curRoom = curRooms.value(device);
        if (curRoom.isEmpty())
            continue;

        QSet<QString> results;
        QMap<int, QString> sorted;

        // sort rooms by RSSI
        foreach (QString room, adjRooms.uniqueKeys()) {
            int r = rssi->rssi(device, room);
            if (r != RssiModel::InvalidRssi)
                sorted[r] = room;
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
        results &= adjRooms.value(curRoom);

        if (results != curOutputs.value(curRoom)) {
            curOutputs[curRoom] = results;
            newOutputs[curRoom] = results;
        }
    }

    if (!newOutputs.isEmpty())
        emit outputsChanged(newOutputs);
}
