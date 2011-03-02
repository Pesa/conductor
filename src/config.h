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

#ifndef CONFIG_H
#define CONFIG_H

#include <QByteArray>
#include <QHash>
#include <QList>
#include <QSet>
#include <QString>

class Config
{
public:
    static int maxRetries() { return 2; }
    static int maxSimultaneousSpeakers() { return 2; }
    static QHash<QString, QByteArray> probesAddresses();
    static QList<QString> roomsNames();
    static QHash<QString, QSet<QString> > roomsTopology();
    static int rpcPort() { return 1234; }
    static int updateInterval() { return 3000; }

private:
    Config() {}
};

#endif // CONFIG_H
