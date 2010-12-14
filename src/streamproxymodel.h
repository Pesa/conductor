#ifndef STREAMPROXYMODEL_H
#define STREAMPROXYMODEL_H

#include <QSortFilterProxyModel>

class StreamProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit StreamProxyModel(QObject *parent = 0);

    int columnCount(const QModelIndex &parent) const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

#endif // STREAMPROXYMODEL_H
