#include "ClientViewer.h"

QList<QString>* Client::get_clients(){return clients;}


void Client::client_cb(pa_context *, const pa_client_info *i, int eol, void * clients)
{
	if (eol < 0) {
		qWarning() << "client callback failure\n";
		return;
	} else if (eol > 0) {
		qDebug() << "client callback done\n";
		return;
	}
	QList<QString>* list = static_cast<QList<QString>*>(clients);
	list->append(i->name);
	
	
	qDebug() << "client #" << i->index << ":" << i->name;
}


void Client::determine_client_available(pa_context *c, pa_operation * o)
{
	if (!(o = pa_context_get_client_info_list(c, client_cb, static_cast<void*> (clients)))) {
			qWarning() << "pa_context_get_client_info_list() failed";
			return;
		}
		
	/*qDebug() << "lista dei nomi dei clients: ";
			for (int j = 0; j < clients->size(); ++j) {
				qDebug() << "client: " << clients->at(j);
			}*/
		pa_operation_unref(o);

}


