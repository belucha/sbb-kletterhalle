#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // create timer for unlocked state
    this->tUnlocked = new QTimer(this);
    this->tUnlocked->setInterval(5000);
    this->tUnlocked->setSingleShot(true);
    // create state machine
    this->smDoor = new QStateMachine(this);
    // create and add states
    this->smDoor->addState(this->sBlocked = new QState(this->smDoor));
    this->smDoor->addState(this->sAlarm = new QState(this->smDoor));
    this->smDoor->addState(this->sUnlocked = new QState(this->smDoor));
    this->smDoor->addState(this->sEntry = new QState(this->smDoor));
    // show state
    this->sBlocked->assignProperty(this->ui->labelState, "text", "door is blocked");
    this->sUnlocked->assignProperty(this->ui->labelState, "text", "door is free");
    this->sAlarm->assignProperty(this->ui->labelState, "text", "alarm");
    this->sEntry->assignProperty(this->ui->labelState, "text", "entry");
    // count alarms and entries
    connect(this->sAlarm, SIGNAL(entered()), this->ui->spinBoxAlarms, SLOT(stepUp()));
    connect(this->sEntry, SIGNAL(entered()), this->ui->spinBoxEntries, SLOT(stepUp()));
    // add transitions
    this->sBlocked->addTransition(this->ui->pushButtonTicket, SIGNAL(clicked()), this->sUnlocked);
    this->sBlocked->addTransition(this->ui->pushButtonEntry, SIGNAL(clicked()), this->sAlarm);
    this->sUnlocked->addTransition(this->ui->pushButtonEntry, SIGNAL(clicked()), this->sEntry);
    this->sUnlocked->addTransition(this->tUnlocked, SIGNAL(timeout()), sBlocked);
    this->sAlarm->addTransition(this->sBlocked);
    this->sEntry->addTransition(this->sBlocked);
    // timer control
    connect(this->sUnlocked, SIGNAL(entered()), this->tUnlocked, SLOT(start()));
    connect(this->sUnlocked, SIGNAL(exited()), this->tUnlocked, SLOT(stop()));
    // set inital state
    this->smDoor->setInitialState(this->sBlocked);
    // and start machine
    this->smDoor->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
