#include "config.h"

QHash<QString, QString> Config::probesAddresses()
{
    QHash<QString, QString> addrs;
    addrs["room A"] = "127.0.0.1";
    return addrs;
}

QHash<QString, QSet<QString> > Config::roomsTopology()
{
    QHash<QString, QSet<QString> > adj;
    adj["room A"] = QSet<QString>();
    return adj;
}
