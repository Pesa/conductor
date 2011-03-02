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

#include <QSharedData>

#include "sink.h"


class SinkData : public QSharedData
{
public:
    uint32_t index;
    QString name;
    QString desc;
};

Sink::Sink(uint32_t index, const QString &name, const QString &desc) :
    data(new SinkData)
{
    data->index = index;
    data->name = name;
    data->desc = desc;
}

Sink::Sink(const Sink &other) :
    data(other.data)
{
}

Sink::~Sink()
{
}

Sink &Sink::operator=(const Sink &other)
{
    if (this != &other)
        data.operator=(other.data);
    return *this;
}

bool Sink::operator==(const Sink &other)
{
    return index() == other.index();
}

bool Sink::operator!=(const Sink &other)
{
    return index() != other.index();
}

uint32_t Sink::index() const        { return data->index; }
QString Sink::name() const          { return data->name; }
QString Sink::description() const   { return data->desc; }
