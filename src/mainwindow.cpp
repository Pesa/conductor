#include <QMessageBox>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    controller(new PAController(this))
{
    ui->setupUi(this);
    ui->sinkInputView->setModel(controller->modelForSinkInputs());
    ui->sinkView->setModel(controller->modelForSinks());

    connect(controller, SIGNAL(error(QString)), SLOT(displayError(QString)));
    connect(controller, SIGNAL(warning(QString)), SLOT(displayWarning(QString)));
    connect(controller, SIGNAL(connected(QString,bool)), SLOT(onPAConnected(QString,bool)));

    QTimer::singleShot(0, controller, SLOT(connectToDaemon()));
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

void MainWindow::onPAConnected(const QString &server, bool local)
{
    QString msg(tr("Connected to %1 PulseAudio daemon @ %2")
                .arg(local ? tr("local") : tr("remote"))
                .arg(server));
    statusBar()->showMessage(msg);
    // TODO: start periodic timer for algo
}
