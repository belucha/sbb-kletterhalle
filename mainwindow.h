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
    static QSignalTransition* addTimeoutTransition(QState* sourceState, int msec, QAbstractState* destState);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    // door state machine releated objects
    QStateMachine* sDoorMachine;
    QState* sDoorBlocked;
    QState* sDoorAlarm;
    QState* sDoorEntry;
    QState* sDoorUnlocked;
    // scanner state machine releated objects
    QStateMachine* sSMachine;
    QState *sSEntryWaitForCode, *sSInfoWaitForCode, *sSInfoRequest, *sSInfoResponse,
           *sSInfoShow, *sSRequestError, *sSEntryRequest, *sSEntryResponse, *sSEntryBusy, *sSRestart;
};

#endif // MAINWINDOW_H
