#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStateMachine>
#include <QState>
#include <QTimer>
#include <QString>
#include <QKeyEvent>
#include <QtNetwork/QNetworkRequest>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    virtual void keyReleaseEvent ( QKeyEvent * event );
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    // door state machine releated objects
    QStateMachine* sDoorMachine;
    QState* sDoorBlocked;
    QState* sDoorAlarm;
    QState* sDoorEntry;
    QState* sDoorUnlocked;
    QTimer* tDoorUnlocked;
    // scanner state machine related objects
    QString* sScannerCode;
    QStateMachine* sScannerMachine;
    QState* sScannerReady;
    QState* sScannerShowInfo;
    QState* sScannerTicket;
};

#endif // MAINWINDOW_H
