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
    // create timer for unlocked state, this timer is started, when a ticket is available but not used
    this->tDoorUnlocked = new QTimer(this);
    this->tDoorUnlocked->setInterval(5000);
    this->tDoorUnlocked->setSingleShot(true);
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
    this->sDoorUnlocked->addTransition(this->tDoorUnlocked, SIGNAL(timeout()), sDoorBlocked);
    this->sDoorAlarm->addTransition(this->sDoorBlocked);
    this->sDoorEntry->addTransition(this->sDoorBlocked);
    // timer control
    connect(this->sDoorUnlocked, SIGNAL(entered()), this->tDoorUnlocked, SLOT(start()));
    connect(this->sDoorUnlocked, SIGNAL(exited()), this->tDoorUnlocked, SLOT(stop()));
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
