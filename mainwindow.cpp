#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // ****************************************************************
    // ** DOOR STATE MACHINE CREATION *********************************
    // ****************************************************************
    // create state machine
    this->sDoorMachine = new QStateMachine(this);
    // create and add states
    this->sDoorBlocked = new QState(this->sDoorMachine);
    this->sDoorAlarm = new QState(this->sDoorMachine);
    this->sDoorUnlocked = new QState(this->sDoorMachine);
    this->sDoorEntry = new QState(this->sDoorMachine);
    // show state
    this->sDoorBlocked->assignProperty(this->ui->labelState, "text", "door is blocked");
    this->sDoorUnlocked->assignProperty(this->ui->labelState, "text", "door is free");
    this->sDoorAlarm->assignProperty(this->ui->labelState, "text", "alarm");
    this->sDoorEntry->assignProperty(this->ui->labelState, "text", "entry");
    // count alarms and entries
    connect(this->sDoorAlarm, SIGNAL(entered()), this->ui->spinBoxAlarms, SLOT(stepUp()));
    connect(this->sDoorEntry, SIGNAL(entered()), this->ui->spinBoxEntries, SLOT(stepUp()));
    // add transitions
    this->sDoorBlocked->addTransition(this->ui->pushButtonTicket, SIGNAL(clicked()), this->sDoorUnlocked);
    this->sDoorBlocked->addTransition(this->ui->pushButtonEntry, SIGNAL(clicked()), this->sDoorAlarm);
    this->sDoorUnlocked->addTransition(this->ui->pushButtonEntry, SIGNAL(clicked()), this->sDoorEntry);
    addTimeoutTransition(this->sDoorUnlocked, 5000, sDoorBlocked);
    this->sDoorAlarm->addTransition(this->sDoorBlocked);
    this->sDoorEntry->addTransition(this->sDoorBlocked);
    // set inital state
    this->sDoorMachine->setInitialState(this->sDoorBlocked);
    // and start machine
    this->sDoorMachine->start();


    // ****************************************************************
    // ** SCANNER STATE MACHINE CREATION ******************************
    // ****************************************************************
}

void MainWindow::keyReleaseEvent ( QKeyEvent * event )
{
    QMainWindow::keyReleaseEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QSignalTransition* MainWindow::addTimeoutTransition(QState* sourceState, int msec, QAbstractState* destState)
{
    QTimer* timer = new QTimer(sourceState);
    timer->setInterval(msec);
    timer->setSingleShot(true);
    connect(sourceState, SIGNAL(entered()), timer, SLOT(start()));
    connect(sourceState, SIGNAL(exited()), timer, SLOT(stop()));
    return sourceState->addTransition(timer, SIGNAL(timeout()), destState);
}
