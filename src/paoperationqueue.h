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

#ifndef PAOPERATIONQUEUE_H
#define PAOPERATIONQUEUE_H

#include <QObject>
#include <QQueue>

class PAOperation;
struct pa_context;

class PAOperationQueue : public QObject
{
    Q_OBJECT

public:
    explicit PAOperationQueue(QObject *parent = 0);

    void enqueue(PAOperation *op);

    bool isRunning() const { return context != 0; }
    void setAbortOnFailure(bool abort) { abortOnFailure = abort; }

public slots:
    void exec(pa_context *c);

signals:
    void error(int errno);
    void finished();

private slots:
    void onOperationError(int errno);
    void onOperationFinished();

private:
    void execHead();

    bool abortOnFailure;
    pa_context *context;
    QQueue<PAOperation*> queue;
};

#endif // PAOPERATIONQUEUE_H
