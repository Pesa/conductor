#include <QMessageBox>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "algorithm.h"
#include "config.h"
#include "pacontroller.h"
#include "probemanager.h"
#include "rssimodel.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    controller(new PAController(this)),
    probe(new ProbeManager(this)),
    rssi(new RssiModel(this)),
    timer(new QTimer(this))
{
    ui->setupUi(this);
    ui->rssiView->setModel(rssi);
    ui->sinkInputView->setModel(controller->modelForSinkInputs());
    ui->sinkView->setModel(controller->modelForSinks());

    timer->setInterval(Config::updateInterval());

    Algorithm *algo = new Algorithm(rssi, this);
    connect(algo, SIGNAL(outputsChanged(QHash<QString,QSet<QString> >)), SLOT(onOutputsChanged(QHash<QString,QSet<QString> >)));

    connect(controller, SIGNAL(error(QString)), SLOT(displayError(QString)));
    connect(controller, SIGNAL(warning(QString)), SLOT(displayWarning(QString)));
    connect(controller, SIGNAL(connected(QString,bool)), SLOT(onPAConnected(QString,bool)));
    connect(probe, SIGNAL(ready()), SLOT(onProbeReady()));
    connect(probe, SIGNAL(rssiChanged(QString,QString,int)), rssi, SLOT(setRssi(QString,QString,int)));
    connect(timer, SIGNAL(timeout()), algo, SLOT(chooseOutputs()));
    connect(ui->monitorButton, SIGNAL(clicked(bool)), SLOT(monitor(bool)));

    QTimer::singleShot(0, controller, SLOT(connectToDaemon()));
    QTimer::singleShot(0, probe, SLOT(connectToProbes()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayError(const QString &msg)
{
    QMessageBox::critical(this, tr("Fatal error"), msg);
}

void MainWindow::displayWarning(const QString &msg)
{
    QMessageBox::warning(this, tr("Warning"), msg);
}

void MainWindow::monitor(bool enabled)
{
    ui->address->setEnabled(!enabled);
    QString device = ui->address->text().toUpper();
    if (enabled) {
        rssi->addDevice(device);
        probe->startMonitoring(device);
        timer->start();
    } else {
        timer->stop();
        probe->stopMonitoring(device);
        rssi->removeDevice(device);
    }
}

void MainWindow::onOutputsChanged(const QHash<QString, QSet<QString> > &outputs)
{
    // we support only one device for now
    Q_ASSERT(outputs.size() == 1);

    QModelIndexList selected = ui->sinkInputView->selectionModel()->selectedRows();
    if (selected.isEmpty())
        return;

    // at most one row can be selected
    Q_ASSERT(selected.size() == 1);

    int row = selected.first().row();
    SinkInput input = controller->modelForSinkInputs()->sinkInputAtRow(row);

    foreach (const QSet<QString> &rooms, outputs) {
        QList<QByteArray> addrs;
        foreach (const QString &room, rooms)
            addrs << probe->addressOfProbe(room);

        // finally move the selected sink input to the chosen speakers
        controller->moveSinkInput(input, addrs);
    }
}

void MainWindow::onPAConnected(const QString &server, bool local)
{
    QString msg(tr("Connected to %1 PulseAudio daemon @ %2")
                .arg(local ? tr("local") : tr("remote"))
                .arg(server));
    statusBar()->showMessage(msg);

    // create a tunnel sink towards each speaker
    foreach (const QString &addr, Config::probesAddresses())
        controller->createTunnel(addr.toLocal8Bit());
}

void MainWindow::onProbeReady()
{
    ui->monitorButton->setEnabled(true);
}
