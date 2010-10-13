#ifndef SINKINPUTVIEWER_H
#define SINKINPUTVIEWER_H

#include <QtDebug>
#include <QCoreApplication>

#include <pulse/context.h>
#include <pulse/glib-mainloop.h>
#include <pulse/introspect.h>
#include <pulse/operation.h>


class SinkInput{

private:
	//int num_sinks_input_available;
	QList<QString>* sinks_input;

public:

	SinkInput(){
		sinks_input = new QList<QString>();
	}	
	
	void determine_sink_input_available(pa_context *, pa_operation *);

	static void sink_input_cb(pa_context *, const pa_sink_input_info *, int , void *);
	
	static void move_cb(pa_context *, int success, void *);

	~SinkInput(){}
};

#endif
