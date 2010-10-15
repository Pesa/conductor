#ifndef SINKVIEWER_H
#define SINKVIEWER_H

#include <QtDebug>
#include <QList>
#include <QString>

#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/operation.h>


class Sink
{
private:
    QList<QString>* sinks;

public:
    Sink(){
        sinks = new QList<QString>();
    }

    void determine_sink_available(pa_context * );

    static void sink_cb(pa_context *, const pa_sink_info *, int , void *);
};

#endif
