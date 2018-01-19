#include "dbaseconnect.h"
#include <QtSql>
#include <QDebug>
#include <QSqlQuery>

DBaseConnect::DBaseConnect(QMap<QString,QString> opt, QObject *parent) : QObject(parent)
{
    config = opt;

}

void DBaseConnect::createConnection()
{

    ///Подключаемся к базе данных АЗС
    bool statusConnect;
    dataOp s;
    typedef QVector<dataOp> vek;
//    qRegisterMetaType<dataOp>("dataOp");
    qRegisterMetaType<vek>("vektor");

    QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE",config.value("conName"));
    db.setHostName(config.value("server"));
    db.setDatabaseName(config.value("basename"));
    db.setUserName(config.value("login"));
    db.setPassword(config.value("password"));
    if(!db.open()) {
        statusConnect=false;
        QString errorString =  db.lastError().text();
        qDebug() <<  "Не возможно подключиться к базе данных." << endl << "Причина:" << errorString;
        emit connectionError(errorString);
    } else {
        statusConnect=true;
        QSqlQuery q = QSqlQuery(db);
//        s = new dataOp;
        q.exec("SELECT s.OPERATOR_ID, TRIM(s.LOGIN), TRIM(s.FIO), TRIM(s.PSWD), s.ISACTIVE from OPERATORS s "
               "where s.OPERATOR_ID>2 "
               "order by s.OPERATOR_ID");
        while(q.next()){

            s.Id=q.value(0).toInt();
            s.login=q.value(1).toString();
            s.fio=q.value(2).toString();
            s.pswd=q.value(3).toString();
            if(q.value(4).toString().trimmed()=="T")
                s.isactive=true;
            else
                s.isactive=false;
            gVector.push_back(s);
        }
        emit sendOperators(gVector);
    }
    emit sendStatus(statusConnect);
    emit fin();
}
