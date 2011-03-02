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

#include "sinkinput.h"


class SinkInputData : public QSharedData
{
public:
    uint32_t index;
    QString name;
    uint32_t client;
    uint32_t sink;
};

SinkInput::SinkInput(uint32_t index, const QString &name, uint32_t client, uint32_t sink) :
    data(new SinkInputData)
{
    data->index = index;
    data->name = name;
    data->client = client;
    data->sink = sink;
}

SinkInput::SinkInput(const SinkInput &other) :
    data(other.data)
{
}

SinkInput::~SinkInput()
{
}

SinkInput &SinkInput::operator=(const SinkInput &other)
{
    if (this != &other)
        data.operator=(other.data);
    return *this;
}

bool SinkInput::operator==(const SinkInput &other)
{
    return index() == other.index();
}

bool SinkInput::operator!=(const SinkInput &other)
{
    return index() != other.index();
}

uint32_t SinkInput::index() const   { return data->index; }
QString SinkInput::name() const     { return data->name; }
uint32_t SinkInput::client() const  { return data->client; }
uint32_t SinkInput::sink() const    { return data->sink; }
