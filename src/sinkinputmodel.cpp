#include <pulse/introspect.h>
#include <pulse/operation.h>

#include "sinkinputmodel.h"


SinkInputModel::SinkInputModel(QObject *parent) :
    QAbstractTableModel(parent),
    clients(new QMap<uint32_t, QString>),
    clientsTemp(0),
    inputs(new QList<SinkInput>),
    inputsTemp(0)
{
}

SinkInputModel::~SinkInputModel()
{
    delete clients;
    if (clientsTemp)
        delete clientsTemp;

    delete inputs;
    if (inputsTemp)
        delete inputsTemp;
}

int SinkInputModel::columnCount(const QModelIndex &) const
{
    return 3;
}

int SinkInputModel::rowCount(const QModelIndex &) const
{
    return inputs->size();
}

QVariant SinkInputModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        SinkInput si = inputs->value(index.row());
        switch (index.column()) {
        case 0: return si.name();
        case 1: return clients->value(si.client(), tr("N/A"));
        case 2: return si.sink();
        }
    }

    return QVariant();
}

QVariant SinkInputModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0: return tr("Name");
            case 1: return tr("Client");
            case 2: return tr("Sink #");
            }
        } else {
            return inputs->value(section).index();
        }
    }

    return QVariant();
}

bool SinkInputModel::populate(pa_context *c)
{
    pa_operation *op;

    op = pa_context_get_sink_input_info_list(c, SinkInputModel::populateSinkInputCallback, this);
    if (op) {
        inputsTemp = new QList<SinkInput>;
        pa_operation_unref(op);

        op = pa_context_get_client_info_list(c, SinkInputModel::populateClientCallback, this);
        if (op) {
            clientsTemp = new QMap<uint32_t, QString>;
            pa_operation_unref(op);
        }

        return true;
    }

    return false;
}

void SinkInputModel::removeClient(uint32_t index)
{
    beginResetModel();
    clients->remove(index);
    endResetModel();
}

void SinkInputModel::removeSinkInput(uint32_t index)
{
    beginResetModel();
    inputs->removeAll(SinkInput(index));
    endResetModel();
}

void SinkInputModel::updateClient(pa_context *c, uint32_t index)
{
    pa_operation *op = pa_context_get_client_info(c, index, SinkInputModel::updateClientCallback, this);
    if (op)
        pa_operation_unref(op);
}

void SinkInputModel::updateSinkInput(pa_context *c, uint32_t index)
{
    pa_operation *op = pa_context_get_sink_input_info(c, index, SinkInputModel::updateSinkInputCallback, this);
    if (op)
        pa_operation_unref(op);
}

void SinkInputModel::populateClientCallback(pa_context *, const pa_client_info *i, int eol, void *userdata)
{
    SinkInputModel *model = static_cast<SinkInputModel*>(userdata);

    if (eol < 0) {
        qWarning("SinkInputModel::populateClientCallback() failure");
        if (model->clientsTemp)
            delete model->clientsTemp;
        return;
    }

    if (eol > 0) {
        model->beginResetModel();
        delete model->clients;
        model->clients = model->clientsTemp;
        model->clientsTemp = 0;
        model->endResetModel();
        return;
    }

    Q_ASSERT(model->clientsTemp);
    model->clientsTemp->insert(i->index, i->name);
}

void SinkInputModel::populateSinkInputCallback(pa_context *, const pa_sink_input_info *i, int eol, void *userdata)
{
    SinkInputModel *model = static_cast<SinkInputModel*>(userdata);

    if (eol < 0) {
        qWarning("SinkInputModel::populateSinkInputCallback() failure");
        if (model->inputsTemp)
            delete model->inputsTemp;
        return;
    }

    if (eol > 0) {
        model->beginResetModel();
        delete model->inputs;
        model->inputs = model->inputsTemp;
        model->inputsTemp = 0;
        model->endResetModel();
        return;
    }

    Q_ASSERT(model->inputsTemp);
    model->inputsTemp->append(SinkInput(i->index, i->name, i->client, i->sink));
}

void SinkInputModel::updateClientCallback(pa_context *, const pa_client_info *i, int eol, void *userdata)
{
    SinkInputModel *model = static_cast<SinkInputModel*>(userdata);

    if (eol != 0) {
        if (eol < 0)
            qWarning("SinkInputModel::updateClientCallback() failure");
        return;
    }

    model->beginResetModel();
    model->clients->insert(i->index, i->name);
    model->endResetModel();
}

void SinkInputModel::updateSinkInputCallback(pa_context *, const pa_sink_input_info *i, int eol, void *userdata)
{
    SinkInputModel *model = static_cast<SinkInputModel*>(userdata);

    if (eol != 0) {
        if (eol < 0)
            qWarning("SinkInputModel::updateSinkInputCallback() failure");
        return;
    }

    SinkInput si(i->index, i->name, i->client, i->sink);
    int idx = model->inputs->indexOf(si);

    model->beginResetModel();
    if (idx == -1)
        model->inputs->append(si);
    else
        model->inputs->replace(idx, si);
    model->endResetModel();
}
