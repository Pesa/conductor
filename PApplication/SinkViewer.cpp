#include "SinkViewer.h"

void Sink::sink_cb(pa_context *c, const pa_sink_info *i, int eol, void * l)
{
	if (eol < 0) {
		qWarning() << "sink callback failure\n";
		return;
	} else if (eol > 0) {
		qDebug() << "sink callback done\n";
		return;
	}
	QList<QString>* list = static_cast<QList<QString>* >(l);
	
	list->append(i->name);
	
	qDebug() << "sink #" << i->index << ":" << i->name;
	
	
	
	
}

void Sink::determine_sink_available(pa_context *c, pa_operation * o)
{
	if (!(o = pa_context_get_sink_info_list(c, Sink::sink_cb, static_cast<void*>(sinks)))) {
			qWarning() << "pa_context_get_sink_info_list() failed";
			return;
		}
	pa_operation_unref(o);
}

