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
            // consider at most a number of speakers equal to maxSimultaneousSpeakers
            if (n >= Config::maxSimultaneousSpeakers())
                break;
            results << i.value();
            ++n;
        }

        // wait a bit more if we don't have enough data, the monitored
        // device might have temporarily fallen out of range
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

        // update only if new outputs differ from the current ones
        if (results != curRooms) {
            curOutputs[device] = results;
            newOutputs[device] = results;
        }
    }

    // notify new outputs
    if (!newOutputs.isEmpty())
        emit outputsChanged(newOutputs);
}
