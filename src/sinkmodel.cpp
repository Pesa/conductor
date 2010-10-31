#include <pulse/introspect.h>
#include <pulse/operation.h>

#include "sinkmodel.h"


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
    pa_operation *op = pa_context_get_sink_info_by_index(c, index, SinkModel::updateSinkCallback, this);
    if (op)
        pa_operation_unref(op);
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

void SinkModel::updateSinkCallback(pa_context *, const pa_sink_info *i, int eol, void *userdata)
{
    SinkModel *model = static_cast<SinkModel*>(userdata);

    if (eol != 0) {
        if (eol < 0)
            qWarning("SinkModel::updateSinkCallback() failure");
        return;
    }

    Sink s(i->index, i->name, i->description);
    int idx = model->sinks->indexOf(s);

    model->beginResetModel();
    if (idx == -1)
        model->sinks->append(s);
    else
        model->sinks->replace(idx, s);
    model->endResetModel();
}
