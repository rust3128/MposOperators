#ifndef OPERATORCHANGED_H
#define OPERATORCHANGED_H

#include <QObject>
#include <QStringList>
#include <QVector>

class OperatorChanged : public QObject
{
    Q_OBJECT
public:
    explicit OperatorChanged(QObject *parent = nullptr);

signals:

public slots:
    void getConnName(QString str);
    void getListSql(QVector <QStringList> lst);
    void sqlExecute();

private:
    QString connName;
    QVector <QStringList> listSql;
};

#endif // OPERATORCHANGED_H
