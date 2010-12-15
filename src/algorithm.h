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
