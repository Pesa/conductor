#include <pulse/introspect.h>
#include <pulse/operation.h>

#include "sinkinputmodel.h"
#include "paoperation.h"


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
        SinkInput si = sinkInputAtRow(index.row());
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
            return sinkInputAtRow(section).index();
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
    ClientInfoOperation *o = new ClientInfoOperation(index, this);
    connect(o, SIGNAL(result(ClientInfoOperation*,const pa_client_info*)),
            SLOT(onClientInfoResult(ClientInfoOperation*,const pa_client_info*)));
    o->exec(c);
}

void SinkInputModel::updateSinkInput(pa_context *c, uint32_t index)
{
    SinkInputInfoOperation *o = new SinkInputInfoOperation(index, this);
    connect(o, SIGNAL(result(SinkInputInfoOperation*,const pa_sink_input_info*)),
            SLOT(onSinkInputInfoResult(SinkInputInfoOperation*,const pa_sink_input_info*)));
    o->exec(c);
}

void SinkInputModel::onClientInfoResult(ClientInfoOperation *o, const pa_client_info *i)
{
    SinkInputModel *model = static_cast<SinkInputModel*>(o->parent());

    model->beginResetModel();
    model->clients->insert(i->index, i->name);
    model->endResetModel();
}

void SinkInputModel::onSinkInputInfoResult(SinkInputInfoOperation *o, const pa_sink_input_info *i)
{
    SinkInput si(i->index, i->name, i->client, i->sink);
    SinkInputModel *model = static_cast<SinkInputModel*>(o->parent());
    int idx = model->inputs->indexOf(si);

    model->beginResetModel();
    if (idx == -1)
        model->inputs->append(si);
    else
        model->inputs->replace(idx, si);
    model->endResetModel();
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
