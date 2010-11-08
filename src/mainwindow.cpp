#include <QMessageBox>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "algorithm.h"
#include "pacontroller.h"
#include "probemanager.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    algo(new Algorithm(this)),
    controller(new PAController(this)),
    probe(new ProbeManager(this)),
    timer(new QTimer(this))
{
    ui->setupUi(this);
    ui->sinkInputView->setModel(controller->modelForSinkInputs());
    ui->sinkView->setModel(controller->modelForSinks());

    timer->setInterval(2000);

    connect(ui->monitorButton, SIGNAL(clicked(bool)), SLOT(monitor(bool)));
    connect(algo, SIGNAL(outputsChanged(QHash<QString,QSet<QString> >)), SLOT(onOutputsChanged(QHash<QString,QSet<QString> >)));
    connect(controller, SIGNAL(error(QString)), SLOT(displayError(QString)));
    connect(controller, SIGNAL(warning(QString)), SLOT(displayWarning(QString)));
    connect(controller, SIGNAL(connected(QString,bool)), SLOT(onPAConnected(QString,bool)));
    connect(probe, SIGNAL(ready()), SLOT(onProbeReady()));
    connect(probe, SIGNAL(rssiChanged(QString,QString,int)), algo, SLOT(setRssi(QString,QString,int)));
    connect(timer, SIGNAL(timeout()), algo, SLOT(chooseOutputs()));

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
    if (enabled) {
        probe->startMonitoring(ui->address->text().toUpper());
        timer->start();
    } else {
        timer->stop();
        probe->stopMonitoring(ui->address->text().toUpper());
    }
}

void MainWindow::onOutputsChanged(const QHash<QString, QSet<QString> > &)
{
    // TODO
}

void MainWindow::onPAConnected(const QString &server, bool local)
{
    QString msg(tr("Connected to %1 PulseAudio daemon @ %2")
                .arg(local ? tr("local") : tr("remote"))
                .arg(server));
    statusBar()->showMessage(msg);
}

void MainWindow::onProbeReady()
{
    ui->monitorButton->setEnabled(true);
}
