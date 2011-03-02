/**********************************************************************
 *
 * Copyright (C) 2011  Davide Pesavento
 *
 * This file is part of Conductor.
 *
 * Conductor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Conductor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Conductor.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "sinkinput.h"

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
    SinkInput input;
    QTimer *timer;
    bool ready;
};

#endif // MAINWINDOW_H
