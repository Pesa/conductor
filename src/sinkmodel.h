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

#ifndef SINKMODEL_H
#define SINKMODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "sink.h"

class SinkInfoOperation;
struct pa_context;
struct pa_sink_info;

class SinkModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit SinkModel(QObject *parent = 0);
    ~SinkModel();

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool populate(pa_context *c);
    void removeSink(uint32_t index);
    void updateSink(pa_context *c, uint32_t index);

private slots:
    void onSinkInfoResult(SinkInfoOperation *o, const pa_sink_info *i);

private:
    static void populateSinkCallback(pa_context *c, const pa_sink_info *i, int eol, void *userdata);

    QList<Sink> *sinks;
    QList<Sink> *sinksTemp;
};

#endif // SINKMODEL_H
