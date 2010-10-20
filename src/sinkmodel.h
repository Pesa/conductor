#ifndef SINKMODEL_H
#define SINKMODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "sink.h"

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

private:
    static void populateSinkCallback(pa_context *c, const pa_sink_info *i, int eol, void *userdata);
    static void updateSinkCallback(pa_context *c, const pa_sink_info *i, int eol, void *userdata);

    QList<Sink> *sinks;
    QList<Sink> *sinksTemp;
};

#endif // SINKMODEL_H
