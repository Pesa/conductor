#ifndef SINKINPUTVIEWER_H
#define SINKINPUTVIEWER_H

#include <QtDebug>
#include <QMap>

#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/operation.h>


class SinkInput
{
private:
    QMap<uint32_t,const char*>* sinks_input;

public:
    SinkInput(){
        sinks_input = new QMap<uint32_t,const char*>();//index & name sink input
    }

    QMap<uint32_t, const char *> get_sinks_input();
    void determine_sink_input_available(pa_context *);
    void move_audio_stream(pa_context*, uint32_t);//move stream from a sink_input to a new_sink specified in the second parameter
    static void sink_input_cb(pa_context *, const pa_sink_input_info *, int , void *);
    static void move_cb(pa_context *, int success, void *);

};

#endif
