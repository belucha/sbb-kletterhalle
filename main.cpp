#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

using namespace json11;

static void jsonTest(void)
{
    //****************************************************************************
    //* SAMPLE JSON PARSE
    //****************************************************************************
    QString test = R"FooFoo(
      {
        "eid": 179,
        "message": "Member was not checked out before! (last update at 2014-10-09 20:56:53)",
        "status": "service",
        "ticket": {
          "authorization": {
            "aid": 12,
            "mnb": 73829473910,
            "tickets": 3,
            "type": "annual",
            "valid": true,
            "validSince": "2014-02-01"
          },
          "checkedIn": true,
          "date": "2014-10-09",
          "lastUpdate": "2014-10-09 20:59:48",
          "tid": 28
        },
        "timestamp": "2014-10-09 20:59:48"
      }
    )FooFoo";
    std::string err;
    Json json = Json::parse(test.toStdString(), err);
    if (!err.empty()) {
        qDebug()<<QString("Failed: %1").arg(err.c_str());
    } else {
        qDebug()<<QString("eid=%1").arg(json["eid"].int_value());
        qDebug()<<QString("status=%1").arg(QString::fromStdString(json["status"].string_value()));
        qDebug()<<QString("ticket.authorization.valid=%1").arg(json["ticket"]["authorization"]["valid"].bool_value());
        qDebug()<<QString("ticket.authorization.mbn=%1").arg((long long int)json["ticket"]["authorization"]["mnb"].number_value());
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    jsonTest();
    return a.exec();
}
