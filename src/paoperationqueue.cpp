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

#include "paoperationqueue.h"
#include "paoperation.h"

PAOperationQueue::PAOperationQueue(QObject *parent) :
    QObject(parent),
    abortOnFailure(true),
    context(0)
{
    connect(this, SIGNAL(error(int)), SLOT(deleteLater()), Qt::QueuedConnection);
    connect(this, SIGNAL(finished()), SLOT(deleteLater()), Qt::QueuedConnection);
}

void PAOperationQueue::enqueue(PAOperation *op)
{
    if (isRunning())
        return;

    op->setParent(this);
    queue.enqueue(op);
}

void PAOperationQueue::exec(pa_context *c)
{
    if (isRunning())
        return;

    context = c;
    execHead();
}

void PAOperationQueue::onOperationError(int errno)
{
    if (abortOnFailure)
        emit error(errno);
    else
        onOperationFinished();
}

void PAOperationQueue::onOperationFinished()
{
    queue.removeFirst();
    execHead();
}

void PAOperationQueue::execHead()
{
    if (queue.isEmpty())
        emit finished();
    else {
        PAOperation *op = queue.head();
        connect(op, SIGNAL(error(int)), SLOT(onOperationError(int)));
        connect(op, SIGNAL(finished()), SLOT(onOperationFinished()));
        op->exec(context);
    }
}
