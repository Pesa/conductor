#include<QList>
#include<QPair>
#include<QString>
#include<QMap>
#include<QMapIterator>
#include<QCoreApplication>
#include<QtDebug>


using namespace std;

static int compare(const QPair<QString, double> a, const QPair<QString, double> b){

	return ( a.second - b.second );
}


static QString calculate_current_room(QMap<QString, double> m){

	QString index_current_room = "UNDEF";
	//initialization max RSSI
	double max = -100000000;
	QMapIterator<QString, double> iterator(m);
	
	while(iterator.hasNext()){
		iterator.next();
		
		if ( ((iterator.value()) != -999) and ((iterator.value()) >= max) )
		{
			max = iterator.value();
			index_current_room = iterator.key();
		}
	}
	qDebug() << "Current_room: " << index_current_room;
	return index_current_room;

}

//return the list of host indexes no null
static QList<QString> get_index_no_null_RSSI(QMap<QString, double> m){
	QList<QString> list;
	QMapIterator<QString, double> iterator(m);
	while(iterator.hasNext()){
		iterator.next();
		if(iterator.value() != -999){
			list.append(iterator.key());
		}
	}
	return list;

}

//implements the AND operation between the list of indexes available and their neighbours
static QList<QString> get_intersected_neighbours(QList<QString> indexes, QMap<QString, QList<QString> > adj, QString room){
	QList<QString> results;
	QMap<QString, QList<QString> >::iterator i = adj.begin();
	i = adj.find(room);
	QList<QString> neighbours = i.value();
	for(int i = 0; i < indexes.count(); i++ ){
		bool found = false;
		for(int j = 0; j < neighbours.count() and !found; j++){
			if(indexes.at(i) == neighbours.at(j)){
				found = true;
				results.append(indexes.at(i));
			}	
		}
	}
	return results;
}

//initial map, a list of indexes with no null RSSI, current room
static QList<QString> get_sink_indexes(QMap<QString, double> map, QList<QString> l, int max_number, QString room, QMap<QString, QList<QString> > adj){

     QList<QString> results;
     int size = l.count();
     QMap<QString, double>::iterator i = map.begin();
     switch(size){
     
     	case 0:
     	//PAUSE
     	break;
     	
     	case 1:      	
		results.append(i.key());
		break;
     	
     	default:
     	QList<QString> restricted_list; 
     	if(size > max_number){
     		//I have to select the best (limited to max_number) indexes RSSI 

     		QList<QPair<QString, double> > pair_list;
     		for(int j = 0; j < l.count(); j++){
     			i = map.find(l.at(j));
     			double RSSI = i.value();
     			QPair<QString, double> pair(l.at(j), RSSI);	
     			pair_list.append(pair);
     		}
     		qSort(pair_list.begin(), pair_list.end(), compare );
     		
     		for(int k = 0; k < max_number; k++){
     			const QString id = (pair_list.at(k)).first;
     			restricted_list.append(id);
     		}
     	}
     	//It's necessary to intersect the indexes list with the neighbours of current room
     	results = get_intersected_neighbours(restricted_list, adj, room);     
      
      }
      return results;
}



int main(int argc, char **argv)
 {
     
     QCoreApplication app(argc, argv);
     
     
     //In map keys = index hosts, double values = RSSI
     QMap<QString, double> map;
     
     map["1"] = -0.0001;
     map["2"] = 0; //It should be the current room
     map["3"] = -999;//-999 is fixed(by MARTY) value for NULL value
     map["4"] = -5.98;
     map["5"] = -0.23;
     
     
     //In map keys = index hosts, a list of adjacent hosts
     QMap<QString, QList<QString> > adjacencies;
     QList<QString> list1;
     list1 << "3" << "4";
     adjacencies["1"] = list1;
     
     QList<QString> list2;
     list2 << "5";
     adjacencies["2"] = list2;
     
     QList<QString> list3;
     list3 << "1" << "4";
     adjacencies["3"] = list3;
     
     QList<QString> list4;
     list4 << "1" << "3" << "5";
     adjacencies["4"] = list4;
     
     QList<QString> list5;
     list5 << "2" << "4";
     adjacencies["5"] = list5;
     
     //MIN number RSSI necessary to proceed to choice
     int min_number_RSSI = 3;
     
     //MAX number speakers activated simultaneously 
     int max_number_speakers_activated = 2;
     
     QString current_room = calculate_current_room(map);
     
     QList<QString> list_indexes_available = get_index_no_null_RSSI(map);
     
     int number_RSSI_available = list_indexes_available.count(); 
     
     //check if there are available data
     if(number_RSSI_available < min_number_RSSI){
     	qDebug()<< "IT's necessary TO WAIT";	
     } 
     
     QList<QString> sink_indexes = get_sink_indexes(map, list_indexes_available, max_number_speakers_activated, current_room, adjacencies);
  
     qDebug()<< "List sink indexes to use for playback:";
     for(int i = 0; i< sink_indexes.count(); i++)
     {
     	qDebug() << sink_indexes.at(i);
     }  
  
  
  return app.exec();     
 }
