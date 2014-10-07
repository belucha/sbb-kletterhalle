#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ****************************************************************
    // ** ADD SERVER CA CERTIFICATE ***********************************
    // ****************************************************************

    QFile caCertFile(":/certificates/ca.raSpBBerry");
    caCertFile.open(QIODevice::ReadOnly);
    QSslCertificate caCertificate(caCertFile.readAll());
    caCertFile.close();

    QFile clientCertFile(":/certificates/client.raSpBBerry");
    clientCertFile.open(QIODevice::ReadOnly);
    QSslCertificate clientCertificate(clientCertFile.readAll());
    clientCertFile.close();

    QFile keyFile(":/certificates/privkey.key");
    keyFile.open(QIODevice::ReadOnly);
    QSslKey privateKey(keyFile.readAll(), QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, QByteArray("raSpBBerry"));
    keyFile.close();

    qDebug()<<caCertificate.issuerInfo(QSslCertificate::Organization);
    qDebug()<<clientCertificate.issuerInfo(QSslCertificate::Organization);
    //    QSslSocket::addDefaultCaCertificate(caCertificate);

    // ****************************************************************
    // ** SETUP NETWORK ACCES RESOURCES (SSL CONFIG) ******************
    // ****************************************************************
    this->manager = new QNetworkAccessManager(this);
    connect(this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkRequestFinished(QNetworkReply*)));
    connect(this->manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(networkRequestSslError(QNetworkReply*,QList<QSslError>)));
//    QNetworkRequest request(QUrl("https://google.com"));
    QNetworkRequest request(QUrl("https://pap10ds.spdns.de:8001/checkin/123445"));
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::AnyProtocol);
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfig.setLocalCertificate(clientCertificate);
    sslConfig.setPrivateKey(privateKey);
    request.setSslConfiguration(sslConfig);
    qDebug()<<request.sslConfiguration().localCertificate().issuerInfo(QSslCertificate::Organization);
    this->manager->get(request);

    // ****************************************************************
    // ** SCANNER STATE MACHINE CREATION ******************************
    // ****************************************************************
    this->sSMachine = new QStateMachine(this);
    this->sSEntryWaitForCode = new QState(this->sSMachine);
    this->sSInfoWaitForCode  = new QState(this->sSMachine);
    this->sSInfoRequest = new QState(this->sSMachine);
    this->sSInfoResponse = new QState(this->sSMachine);
    this->sSInfoShow = new QState(this->sSMachine);
    this->sSRequestError = new QState(this->sSMachine);
    this->sSEntryRequest = new QState(this->sSMachine);
    this->sSEntryResponse = new QState(this->sSMachine);
    this->sSEntryBusy = new QState(this->sSMachine);
    this->sSRestart = new QState(this->sSMachine);

    // ****************************************************************
    // ** DOOR STATE MACHINE CREATION *********************************
    // ****************************************************************
    this->sDoorMachine = new QStateMachine(this);
    this->sDoorBlocked = new QState(this->sDoorMachine);
    this->sDoorAlarm = new QState(this->sDoorMachine);
    this->sDoorUnlocked = new QState(this->sDoorMachine);
    this->sDoorEntry = new QState(this->sDoorMachine);
    this->sDoorTicketUnused = new QState(this->sDoorMachine);

    // ****************************************************************
    // ** DEFINE ALL POSSIBLE TRANSITIONS *****************************
    // ****************************************************************
    this->sDoorBlocked->addTransition(this->sSEntryResponse, SIGNAL(entered()), this->sDoorUnlocked);
    this->sDoorBlocked->addTransition(this->ui->pushButtonEntry, SIGNAL(clicked()), this->sDoorAlarm);
    this->sDoorUnlocked->addTransition(this->ui->pushButtonEntry, SIGNAL(clicked()), this->sDoorEntry);
    addTimeoutTransition(this->sDoorUnlocked, 5000, this->sDoorTicketUnused);
    this->sDoorAlarm->addTransition(this->sDoorBlocked);
    this->sDoorEntry->addTransition(this->sDoorBlocked);
    this->sDoorTicketUnused->addTransition(this->sDoorBlocked);

    this->sSEntryWaitForCode->addTransition(this->ui->pushButtonTicket, SIGNAL(clicked()), this->sSEntryRequest);
    this->sSEntryWaitForCode->addTransition(this->ui->pushButtonInfo, SIGNAL(clicked()), this->sSInfoWaitForCode);
    this->sSInfoWaitForCode->addTransition(this->ui->pushButtonInfo, SIGNAL(clicked()), this->sSEntryWaitForCode);
    addTimeoutTransition(this->sSInfoWaitForCode, 5000, this->sSEntryWaitForCode);
    this->sSInfoWaitForCode->addTransition(this->ui->pushButtonTicket, SIGNAL(clicked()), this->sSInfoRequest);
    addTimeoutTransition(this->sSInfoRequest, 150, this->sSInfoShow);
    addTimeoutTransition(this->sSInfoShow, 5000, this->sSEntryWaitForCode);
    this->sSInfoShow->addTransition(this->ui->pushButtonInfo, SIGNAL(clicked()), this->sSEntryWaitForCode);

    addTimeoutTransition(this->sSEntryRequest, 150, this->sSEntryResponse); // TODO: exchange by real json request
    this->sSEntryResponse->addTransition(this->sDoorUnlocked, SIGNAL(exited()), this->sSRestart);
    this->sSEntryResponse->addTransition(this->ui->pushButtonTicket, SIGNAL(clicked()), this->sSEntryBusy);
    this->sSEntryBusy->addTransition(this->sDoorUnlocked, SIGNAL(exited()), this->sSRestart);
    addTimeoutTransition(this->sSEntryBusy, 1500, this->sSEntryResponse);   // show busy message for 1.5seconds
    this->sSRestart->addTransition(this->sSEntryWaitForCode);

    // ****************************************************************
    // ** SHOW STATES AND ANIMATE PROPERTIES **************************
    // ****************************************************************
    this->sDoorBlocked->assignProperty(this->ui->labelStateDoor, "text", "door is blocked");
    this->sDoorUnlocked->assignProperty(this->ui->labelStateDoor, "text", "door is free");
    this->sDoorAlarm->assignProperty(this->ui->labelStateDoor, "text", "alarm");
    this->sDoorEntry->assignProperty(this->ui->labelStateDoor, "text", "entry");
    this->sDoorTicketUnused->assignProperty(this->ui->labelStateDoor, "text", "ticket was not used!");

    this->sSEntryWaitForCode->assignProperty(this->ui->labelStateScanner, "text",  "Please show your ticket!");
    this->sSInfoWaitForCode->assignProperty(this->ui->labelStateScanner, "text",  "info: Please show your ticket");
    this->sSInfoRequest->assignProperty(this->ui->labelStateScanner, "text",  "info: request");
    this->sSInfoResponse->assignProperty(this->ui->labelStateScanner, "text",  "info: waiting for server response");
    this->sSInfoShow->assignProperty(this->ui->labelStateScanner, "text",  "info: your ticket is valid until");
    this->sSRequestError->assignProperty(this->ui->labelStateScanner, "text",  "request error, repeat");
    this->sSEntryRequest->assignProperty(this->ui->labelStateScanner, "text",  "waiting for server response");
    this->sSEntryResponse->assignProperty(this->ui->labelStateScanner, "text",  "you are allowed, please enter");
    this->sSEntryBusy->assignProperty(this->ui->labelStateScanner, "text",  "busy please wait");
    this->sSRestart->assignProperty(this->ui->labelStateScanner, "text",  "restart");

    // ****************************************************************
    // ** COUNT SOME EVENTS *******************************************
    // ****************************************************************
    connect(this->sDoorAlarm, SIGNAL(entered()), this->ui->spinBoxAlarms, SLOT(stepUp()));
    connect(this->sDoorEntry, SIGNAL(entered()), this->ui->spinBoxEntries, SLOT(stepUp()));
    connect(this->sDoorTicketUnused, SIGNAL(entered()), this->ui->spinBoxUnused, SLOT(stepUp()));

    // ****************************************************************
    // ** SET INITAL STATES AND RUN THE STATE MACHINES ****************
    // ****************************************************************
    this->sSMachine->setInitialState(this->sSEntryWaitForCode);
    this->sDoorMachine->setInitialState(this->sDoorBlocked);
    this->sSMachine->start();
    this->sDoorMachine->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::networkRequestFinished(QNetworkReply* reply)
{
    qDebug()<<QString(reply->readAll());
    qDebug()<<reply->errorString();
}

void MainWindow::networkRequestSslError(QNetworkReply* reply, const QList<QSslError>& errors)
{
    qDebug()<<QString(reply->readAll());
    qDebug()<<errors;
}

//
// Helper routine the adds a timeout transition between two states
//
QSignalTransition* MainWindow::addTimeoutTransition(QState* sourceState, int msec, QAbstractState* destState)
{
    QTimer* timer = new QTimer(sourceState);
    timer->setInterval(msec);
    timer->setSingleShot(true);
    connect(sourceState, SIGNAL(entered()), timer, SLOT(start()));
    connect(sourceState, SIGNAL(exited()), timer, SLOT(stop()));
    return sourceState->addTransition(timer, SIGNAL(timeout()), destState);
}
