#ifndef DBASECONNECT_H
#define DBASECONNECT_H

#include <QObject>
#include <QMap>
#include <QVector>
#include "operatorsdata.h"


class DBaseConnect : public QObject
{
    Q_OBJECT
public:
    explicit DBaseConnect(QMap<QString,QString> opt, QObject *parent = nullptr);

signals:
    void connectionError(QString errorMeaage);
    void fin();
    void sendStatus(bool);
    void sendOperators(QVector<dataOp>);

public slots:
    void createConnection();

private:
    QMap<QString, QString> config;
    bool statusConnect;
//    dataOp s;
    QVector<dataOp> gVector;


};


#endif // DBASECONNECT_H
