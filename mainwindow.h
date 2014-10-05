#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStateMachine>
#include <QState>
#include <QTimer>
#include <QtNetwork/QNetworkRequest>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStateMachine* smDoor;
    QState* sBlocked;
    QState* sAlarm;
    QState* sEntry;
    QState* sUnlocked;
    QTimer* tUnlocked;
};

#endif // MAINWINDOW_H
