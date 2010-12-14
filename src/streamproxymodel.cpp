#include "streamproxymodel.h"

StreamProxyModel::StreamProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

int StreamProxyModel::columnCount(const QModelIndex &) const
{
    return 2;
}

bool StreamProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex i = sourceModel()->index(sourceRow, 1, sourceParent);
    QString client = sourceModel()->data(i).toString();

    return !client.contains("pulseaudio", Qt::CaseInsensitive);
}
