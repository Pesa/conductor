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

#ifndef SINKINPUT_H
#define SINKINPUT_H

#include <QSharedDataPointer>
#include <QString>

#include <pulse/def.h>

class SinkInputData;

class SinkInput
{
public:
    explicit SinkInput(uint32_t index = -1, const QString &name = QString(),
                       uint32_t client = -1, uint32_t sink = -1);
    SinkInput(const SinkInput &);
    ~SinkInput();

    SinkInput &operator=(const SinkInput &);
    bool operator==(const SinkInput &);
    bool operator!=(const SinkInput &);

    uint32_t index() const;
    QString name() const;
    uint32_t client() const;
    uint32_t sink() const;

private:
    QSharedDataPointer<SinkInputData> data;
};

#endif // SINKINPUT_H
