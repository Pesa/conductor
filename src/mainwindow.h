#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "pacontroller.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void displayError(const QString &msg);
    void displayWarning(const QString &msg);
    void onPAConnected(const QString &server, bool local);

private:
    Ui::MainWindow *ui;
    PAController *controller;
};

#endif // MAINWINDOW_H
