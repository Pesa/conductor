#include "SinkInputViewer.h"

void SinkInput::move_cb(pa_context *, int success, void *)
{
	if (success) {
		qDebug() << "move successful";
	} else {
		qWarning() << "move failed";
	}
}

void SinkInput::sink_input_cb(pa_context *c, const pa_sink_input_info *i, int eol, void * list)
{
	if (eol < 0) {
		qWarning() << "sink input callback failure\n";
		return;
	} else if (eol > 0) {
		qDebug() << "sink input callback done\n";
		return;
	}
	
	QList<QString>* sinks_input = static_cast<QList<QString>*> (list);
	sinks_input -> append(i->name);
	
	qDebug() << "sink input #" << i->index << ":" << i->name;

	/*if (strcasecmp(i->name, "audio stream") == 0) {
		qDebug() << "moving sink input #" << i->index << "to sink #1 ...";
		pa_operation *o;
		if (!(o = pa_context_move_sink_input_by_index(c, i->index, 1, move_cb, NULL))) {
			qWarning() << "pa_context_move_sink_input_by_index() failed";
			return;
		}
		pa_operation_unref(o);
	}*/
}


void SinkInput::determine_sink_input_available(pa_context *c, pa_operation * o)
{
	if (!(o = pa_context_get_sink_input_info_list(c, sink_input_cb, static_cast<void*>(sinks_input)))) {
			qWarning() << "pa_context_get_sink_input_info_list() failed";
			return;
		}
		pa_operation_unref(o);
}
