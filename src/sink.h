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

#ifndef SINK_H
#define SINK_H

#include <QSharedDataPointer>
#include <QString>

#include <pulse/def.h>

class SinkData;

class Sink
{
public:
    explicit Sink(uint32_t index = -1, const QString &name = QString(),
                  const QString &desc = QString());
    Sink(const Sink &);
    ~Sink();

    Sink &operator=(const Sink &);
    bool operator==(const Sink &);
    bool operator!=(const Sink &);

    uint32_t index() const;
    QString name() const;
    QString description() const;

private:
    QSharedDataPointer<SinkData> data;
};

#endif // SINK_H
