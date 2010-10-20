#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pacontroller.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sinkInputView->setModel(PAController::modelForSinkInputs());
    ui->sinkView->setModel(PAController::modelForSinks());

    connect(PAController::instance(), SIGNAL(error(QString)), SLOT(displayError(QString)));
    connect(PAController::instance(), SIGNAL(warning(QString)), SLOT(displayWarning(QString)));
    connect(PAController::instance(), SIGNAL(connected(QString,bool)), SLOT(onPAConnected(QString,bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
    PAController::destroy();
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
}
