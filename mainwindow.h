#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStateMachine>
#include <QState>
#include <QTimer>
#include <QString>
#include <QDebug>
#include <QKeyEvent>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    static QSignalTransition* addTimeoutTransition(QState* sourceState, int msec, QAbstractState* destState);
    ~MainWindow();
private slots:
    void networkRequestFinished(QNetworkReply* reply);
    void networkRequestSslError(QNetworkReply* reply, const QList<QSslError>& errors);
private:
    Ui::MainWindow *ui;
    // network stuff
    QNetworkAccessManager* manager;
    // door state machine releated objects
    QStateMachine* sDoorMachine;
    QState* sDoorBlocked;
    QState* sDoorAlarm;
    QState* sDoorEntry;
    QState* sDoorUnlocked;
    QState* sDoorTicketUnused;
    // scanner state machine releated objects
    QStateMachine* sSMachine;
    QState *sSEntryWaitForCode, *sSInfoWaitForCode, *sSInfoRequest, *sSInfoResponse;
    QState *sSInfoShow, *sSRequestError, *sSEntryRequest, *sSEntryResponse, *sSEntryBusy, *sSRestart;
};

#endif // MAINWINDOW_H
