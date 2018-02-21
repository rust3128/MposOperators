#include "operatorchanged.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "loggingcategories.h"

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
    if(connName.length()==0){

        return;
    }

    QSqlDatabase dbazs = QSqlDatabase::database(connName);
    QSqlQuery q = QSqlQuery(dbazs);
    for(int i=0; i<listSql.size();++i){
        q.prepare(listSql.at(i).at(1));
        if(!q.exec()){
            qCritical(logCritical()) << "Не удалось выполнить: " << q.lastError().text();
        } else {
            qInfo(logInfo()) << "Выполнено: " << listSql.at(i).at(0);
        }
    }
    q.exec("commit");
    emit finish();
}
