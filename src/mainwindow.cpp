#include <QMessageBox>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "algorithm.h"
#include "config.h"
#include "pacontroller.h"
#include "probemanager.h"
#include "rssimodel.h"
#include "streamproxymodel.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    controller(new PAController(this)),
    probe(new ProbeManager(this)),
    rssi(new RssiModel(this)),
    proxy(new StreamProxyModel(this)),
    timer(new QTimer(this)),
    ready(false)
{
    ui->setupUi(this);
    ui->rssiView->setModel(rssi);
    ui->sinkInputView->setModel(controller->modelForSinkInputs());
    ui->sinkView->setModel(controller->modelForSinks());

    // setup the combo box for audio stream selection:
    //   1. set the model
    proxy->setSourceModel(controller->modelForSinkInputs());
    ui->stream->setModel(proxy);
    //   2. set the view
    QTableView *view = new QTableView;
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->stream->setView(view);

    // configure the timer interval
    timer->setInterval(Config::updateInterval());

    // initialize the main algorithm
    Algorithm *algo = new Algorithm(rssi, this);
    connect(algo, SIGNAL(outputsChanged(QHash<QString,QSet<QString> >)), SLOT(onOutputsChanged(QHash<QString,QSet<QString> >)));

    // connect signals and slots
    connect(controller, SIGNAL(error(QString)), SLOT(displayError(QString)));
    connect(controller, SIGNAL(warning(QString)), SLOT(displayWarning(QString)));
    connect(controller, SIGNAL(connected(QString,bool)), SLOT(onPAConnected(QString,bool)));
    connect(probe, SIGNAL(ready()), SLOT(onProbeReady()));
    connect(probe, SIGNAL(rssiChanged(QString,QString,int)), rssi, SLOT(setRssi(QString,QString,int)));
    connect(timer, SIGNAL(timeout()), algo, SLOT(chooseOutputs()));
    connect(ui->monitorButton, SIGNAL(clicked(bool)), SLOT(monitor(bool)));
    connect(ui->testMode, SIGNAL(toggled(bool)), SLOT(onTestModeToggled(bool)));
    connect(ui->testMode, SIGNAL(toggled(bool)), rssi, SLOT(setEditable(bool)));

    // schedule delayed initialization of PAController and ProbeManager
    // (note: the connected slots will be invoked as soon as control returns to Qt event loop)
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
    QString device = ui->address->text().toUpper();
    int idx = ui->stream->currentIndex();

    if (enabled) {
        if (device.isEmpty() || idx < 0) {
            ui->monitorButton->setChecked(false);
            return;
        }

        // figure out which sink input has been selected through the combo box
        int row = proxy->mapToSource(proxy->index(idx, 0)).row();
        input = controller->modelForSinkInputs()->sinkInputAtRow(row);

        // start monitoring
        rssi->addDevice(device);
        probe->startMonitoring(device);
        timer->start();
    } else {
        // stop monitoring
        timer->stop();
        probe->stopMonitoring(device);
        rssi->removeDevice(device);
    }

    // enable/disable some UI widgets
    ui->address->setEnabled(!enabled);
    ui->stream->setEnabled(!enabled);
    ui->testMode->setEnabled(!enabled);
}

void MainWindow::onOutputsChanged(const QHash<QString, QSet<QString> > &outputs)
{
    // we support only one device for now
    Q_ASSERT(outputs.size() == 1);

    foreach (const QSet<QString> &rooms, outputs) {
        QList<QByteArray> addrs;
        foreach (const QString &room, rooms)
            addrs << probe->addressOfProbe(room);

        // move the selected sink input to the current output speakers
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
    foreach (const QByteArray &addr, Config::probesAddresses())
        controller->createTunnel(addr);
}

void MainWindow::onProbeReady()
{
    ready = true;
    ui->monitorButton->setEnabled(true);
}

void MainWindow::onTestModeToggled(bool checked)
{
    ui->monitorButton->setEnabled(checked || ready);
}
