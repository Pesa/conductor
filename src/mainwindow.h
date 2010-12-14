#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTimer;
class PAController;
class ProbeManager;
class RssiModel;
class StreamProxyModel;

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
    void monitor(bool enabled);
    void onOutputsChanged(const QHash<QString, QSet<QString> > &outputs);
    void onPAConnected(const QString &server, bool local);
    void onProbeReady();
    void onTestModeToggled(bool checked);

private:
    Ui::MainWindow *ui;
    PAController *controller;
    ProbeManager *probe;
    RssiModel *rssi;
    StreamProxyModel *proxy;
    QTimer *timer;
    bool ready;
};

#endif // MAINWINDOW_H
