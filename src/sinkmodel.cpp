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

#include <pulse/introspect.h>
#include <pulse/operation.h>

#include "sinkmodel.h"
#include "paoperation.h"


SinkModel::SinkModel(QObject *parent) :
    QAbstractTableModel(parent),
    sinks(new QList<Sink>),
    sinksTemp(0)
{
}

SinkModel::~SinkModel()
{
    delete sinks;
    if (sinksTemp)
        delete sinksTemp;
}

int SinkModel::columnCount(const QModelIndex &) const
{
    return 2;
}

int SinkModel::rowCount(const QModelIndex &) const
{
    return sinks->size();
}

QVariant SinkModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        Sink s = sinks->value(index.row());
        switch (index.column()) {
        case 0: return s.name();
        case 1: return s.description();
        }
    }

    return QVariant();
}

QVariant SinkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0: return tr("Name");
            case 1: return tr("Description");
            }
        } else {
            return sinks->value(section).index();
        }
    }

    return QVariant();
}

bool SinkModel::populate(pa_context *c)
{
    pa_operation *op;

    if (!(op = pa_context_get_sink_info_list(c, SinkModel::populateSinkCallback, this)))
        return false;

    pa_operation_unref(op);
    sinksTemp = new QList<Sink>;
    return true;
}

void SinkModel::removeSink(uint32_t index)
{
    beginResetModel();
    sinks->removeAll(Sink(index));
    endResetModel();
}

void SinkModel::updateSink(pa_context *c, uint32_t index)
{
    SinkInfoOperation *o = new SinkInfoOperation(index, this);
    connect(o, SIGNAL(result(SinkInfoOperation*,const pa_sink_info*)),
            SLOT(onSinkInfoResult(SinkInfoOperation*,const pa_sink_info*)));
    o->exec(c);
}

void SinkModel::onSinkInfoResult(SinkInfoOperation *o, const pa_sink_info *i)
{
    Sink s(i->index, i->name, i->description);
    SinkModel *model = static_cast<SinkModel*>(o->parent());
    int idx = model->sinks->indexOf(s);

    model->beginResetModel();
    if (idx == -1)
        model->sinks->append(s);
    else
        model->sinks->replace(idx, s);
    model->endResetModel();
}

void SinkModel::populateSinkCallback(pa_context *, const pa_sink_info *i, int eol, void *userdata)
{
    SinkModel *model = static_cast<SinkModel*>(userdata);

    if (eol < 0) {
        qWarning("SinkModel::populateSinkCallback() failure");
        if (model->sinksTemp)
            delete model->sinksTemp;
        return;
    }

    if (eol > 0) {
        model->beginResetModel();
        delete model->sinks;
        model->sinks = model->sinksTemp;
        model->sinksTemp = 0;
        model->endResetModel();
        return;
    }

    Q_ASSERT(model->sinksTemp);
    model->sinksTemp->append(Sink(i->index, i->name, i->description));
}
