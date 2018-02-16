#include "operatorchanged.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

OperatorChanged::OperatorChanged(QObject *parent) : QObject(parent)
{

}

void OperatorChanged::getConnName(QString str)
{
    connName = str;
}

void OperatorChanged::getListSql(QVector<QStringList> lst)
{
    listSql = lst;
}

void OperatorChanged::sqlExecute()
{
    qDebug() << "ConnName in Theard" << connName;
    if(connName.length()==0){
        qDebug() << "ConnName is empty";
        return;
    }

    QSqlDatabase dbazs = QSqlDatabase::database(connName);
    QSqlQuery q = QSqlQuery(dbazs);
    for(int i; i<listSql.size();++i){
//        qDebug() << "Comment SQL: " << listSql.at(i).at(0);
//        qDebug() << "Text SQL: " << listSql.at(i).at(1);
        q.prepare(listSql.at(i).at(1));
        if(!q.exec()) qDebug() << q.lastError().text();
//        qDebug() << q.executedQuery();
    }
}
