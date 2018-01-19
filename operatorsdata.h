#ifndef OPERATORSDATA_H
#define OPERATORSDATA_H
#include <QString>

struct dataOp {
        int Id;
        QString login;
        QString fio;
        QString pswd;
        bool isactive;
    };
Q_DECLARE_METATYPE(dataOp)

#endif // OPERATORSDATA_H
