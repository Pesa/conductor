#include "config.h"

QHash<QString, QByteArray> Config::probesAddresses()
{
    QHash<QString, QByteArray> addrs;
    addrs["room A"] = "127.0.0.1";
    addrs["room B"] = "10.42.43.1";
    return addrs;
}

QList<QString> Config::roomsNames()
{
    return probesAddresses().uniqueKeys();
}

QHash<QString, QSet<QString> > Config::roomsTopology()
{
    QHash<QString, QSet<QString> > adj;

    QSet<QString> roomA;
    roomA << "room B";
    adj["room A"] = roomA;

    QSet<QString> roomB;
    roomB << "room A";
    adj["room B"] = roomB;

    return adj;
}
