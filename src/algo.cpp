#include <limits>
#include <QHash>
#include <QList>
#include <QPair>
#include <QMap>
#include <QtDebug>

#include "probemanager.h"

#define NULL_RSSI -999


static int compare(const QPair<QString, double> a, const QPair<QString, double> b)
{
    return a.second - b.second;
}

static QString calculate_current_room(QHash<QString, int> map)
{
    int maxRssi = std::numeric_limits<int>::min();
    QString room;

    QHashIterator<QString, int> i(map);
    while (i.hasNext()) {
        i.next();
        if (i.value() != NULL_RSSI && i.value() > maxRssi) {
            maxRssi = i.value();
            room = i.key();
        }
    }

    return room;
}

//return the list of host indexes no null
static QList<QString> get_index_no_null_RSSI(QHash<QString, int> m)
{
    QList<QString> list;
    QHashIterator<QString, int> iterator(m);
    while(iterator.hasNext()) {
        iterator.next();
        if(iterator.value() != NULL_RSSI){
            list.append(iterator.key());
        }
    }
    return list;
}

//implements the AND operation between the list of indexes available and their neighbours
static QList<QString> get_intersected_neighbours(QList<QString> indexes, QMap<QString, QList<QString> > adj, QString room)
{
    QList<QString> results;

    QList<QString> neighbours = adj.value(room);

    for (int i = 0; i < indexes.count(); i++) {
        bool found = false;
        for(int j = 0; j < neighbours.count() and !found; j++){
            if(indexes.at(i) == neighbours.at(j)){
                found = true;
                results.append(indexes.at(i));
            }
        }
    }
    return results;
}

//initial map, a list of indexes with no null RSSI, current room
static QList<QString> get_sink_indexes(QHash<QString, int> map, // RSSI attuali
                                       QList<QString> l, // lista stanze con RSSI non nullo
                                       int max_number, // n. max di diffusori contemporanei
                                       QString room, // stanza attuale
                                       QMap<QString, QList<QString> > adj) // adiacenza stanze
{
    QList<QString> results;
    int size = l.count();
    QHash<QString, int>::iterator i = map.begin();

    switch (size) {
    case 0:
        // TODO: pause
        break;

    case 1:
        results.append(i.key());
        break;

    default:
        QList<QString> restricted_list;
        if(size > max_number){
            //I have to select the best (limited to max_number) indexes RSSI

            QList<QPair<QString, double> > pair_list;
            for(int j = 0; j < l.count(); j++){
                i = map.find(l.at(j));
                double rssi = i.value();
                QPair<QString, double> pair(l.at(j), rssi);
                pair_list.append(pair);
            }
            qSort(pair_list.begin(), pair_list.end(), compare);

            for(int k = 0; k < max_number; k++){
                QString id = (pair_list.at(k)).first;
                restricted_list.append(id);
            }
        }
        //It's necessary to intersect the indexes list with the neighbours of current room
        results = get_intersected_neighbours(restricted_list, adj, room);
    }

    return results;
}

void algo_test()
{
    ProbeManager man;

    // map: room name => RSSI
    QHash<QString, int> map = man.rssiMap("00:23:D6:9A:DC:25");

    // map: room name => list of adjacent rooms (maybe use a QSet?)
    QMap<QString, QList<QString> > adjacencies;

    QList<QString> list1;
    list1 << "3" << "4";
    adjacencies["1"] = list1;

    QList<QString> list2;
    list2 << "5";
    adjacencies["2"] = list2;

    QList<QString> list3;
    list3 << "1" << "4";
    adjacencies["3"] = list3;

    QList<QString> list4;
    list4 << "1" << "3" << "5";
    adjacencies["4"] = list4;

    QList<QString> list5;
    list5 << "2" << "4";
    adjacencies["5"] = list5;

    // min number RSSI necessary to proceed to choice
    const int min_number_RSSI = 3;

    // max number speakers activated simultaneously
    const int max_number_speakers_activated = 2;

    QString current_room = calculate_current_room(map);

    QList<QString> available = get_index_no_null_RSSI(map);
    if (available.count() < min_number_RSSI) {
        qDebug()<< "It's necessary TO WAIT";
        return;
    }

    QList<QString> sinks = get_sink_indexes(map, available, max_number_speakers_activated, current_room, adjacencies);
    qDebug() << "List sink indexes to use for playback:" << sinks;
}
