#ifndef SINKINPUTMODEL_H
#define SINKINPUTMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QMap>

#include "sinkinput.h"

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

private:
    static void populateClientCallback(pa_context *c, const pa_client_info *i, int eol, void *userdata);
    static void populateSinkInputCallback(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata);
    static void updateClientCallback(pa_context *c, const pa_client_info *i, int eol, void *userdata);
    static void updateSinkInputCallback(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata);

    QMap<uint32_t, QString> *clients;
    QMap<uint32_t, QString> *clientsTemp;
    QList<SinkInput> *inputs;
    QList<SinkInput> *inputsTemp;
};

#endif // SINKINPUTMODEL_H
