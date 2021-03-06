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
