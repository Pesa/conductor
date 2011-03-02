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

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QHash>
#include <QObject>
#include <QSet>

class RssiModel;

class Algorithm : public QObject
{
    Q_OBJECT

public:
    explicit Algorithm(const RssiModel *model, QObject *parent = 0);

public slots:
    /*!
      Run the main algorithm that chooses in which rooms the audio stream
      should be played. New outputs, if changed, are notified through
      the outputsChanged() signal.
    */
    void chooseOutputs();

signals:
    /*!
      This signal is emitted when the output speakers
      chosen by the algorithm have changed.

      \param outputs A hash table containing the new outputs for each device.
    */
    void outputsChanged(const QHash<QString, QSet<QString> > &outputs);

private:
    int retryCount;
    const RssiModel *rssi;

    /* map: room name => adjoining rooms */
    QHash<QString, QSet<QString> > adjRooms;

    /* map: device address => current output rooms */
    QHash<QString, QSet<QString> > curOutputs;
};

#endif // ALGORITHM_H
