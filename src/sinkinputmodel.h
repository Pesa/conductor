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

#ifndef SINKINPUTMODEL_H
#define SINKINPUTMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QMap>

#include "sinkinput.h"

class ClientInfoOperation;
class SinkInputInfoOperation;
struct pa_context;
struct pa_client_info;
struct pa_sink_input_info;

class SinkInputModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit SinkInputModel(QObject *parent = 0);
    ~SinkInputModel();

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    SinkInput sinkInputAtRow(int row) const { return inputs->value(row); }

    bool populate(pa_context *c);
    void removeClient(uint32_t index);
    void removeSinkInput(uint32_t index);
    void updateClient(pa_context *c, uint32_t index);
    void updateSinkInput(pa_context *c, uint32_t index);

private slots:
    void onClientInfoResult(ClientInfoOperation *o, const pa_client_info *i);
    void onSinkInputInfoResult(SinkInputInfoOperation *o, const pa_sink_input_info *i);

private:
    static void populateClientCallback(pa_context *c, const pa_client_info *i, int eol, void *userdata);
    static void populateSinkInputCallback(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata);

    QMap<uint32_t, QString> *clients;
    QMap<uint32_t, QString> *clientsTemp;
    QList<SinkInput> *inputs;
    QList<SinkInput> *inputsTemp;
};

#endif // SINKINPUTMODEL_H
