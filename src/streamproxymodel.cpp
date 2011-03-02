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
