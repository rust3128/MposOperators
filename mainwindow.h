#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QSqlRecord>
#include <QLabel>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QProgressDialog>
#include <QTableWidgetItem>
#include <QThread>
#include <QObject>

#include "operatorsdata.h"


struct userdata
{
    int id;
    QString fio;
};

    typedef  QVector<QStringList> sqlList;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSqlRecord user, QWidget *parent = 0);
    ~MainWindow();

signals:
    void sendConnName(QString);     //Передаем имя соедиения
    void sendChangeList(QVector<QStringList>);

private slots:
    void on_actionUsers_triggered();
    void on_tableView_doubleClicked(const QModelIndex &idx);
    void startDBConnect();
    void finishDBConnect();
    void errogConnectInfo(QString str);
    void getStaus(bool status);
    void getTableOperators(QVector<dataOp> tblOp);
    void getCurrentOperators(int curID);
    void transacionStart();     //Начинаем транзакции
    void finishChange();
    void filterSet();
    void on_pushButton_clicked();
    void on_pushButtonApplay_clicked();
    void on_pushButtonActive_clicked();
    void on_tableWidget_itemSelectionChanged();
    void on_pushButtonOtherAzs_clicked();

    void on_actionListConnect_triggered();

    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
    userdata currentUser;
    QLabel *labelUser;
    QSqlQueryModel *modelTerminals;
    QMap<QString, QString> azsConn;
    QProgressDialog *progress;
    bool isConnected;
    QThread *thread, *modify;
    QModelIndex idxTerm;
    dataOp s;
    QVector<dataOp> opVector;
    QStringList insOp;
    QVector <QStringList> listChange;
    QIcon icoWork = QIcon(":/Images/user_accept.png");
    QIcon icoDel = QIcon(":/Images/user_delete.png");
    QTableWidgetItem *item; //Текущий оператора.
    int currentOperator;    //ID оператора на АЗС на котолрого открыта смена.
    int currIdConn;         //текущее подключене к ЦБ
    QString currConnectName;
private:
    void closeEvent(QCloseEvent *event);
    void infoUser2StatusBar();
    void openCentralDB(void);
    void createUI();
    void setupTerminalModel();
    void createUIOperarors();
    void applayAzs();
    void enabledApplay();
    void otherAzs();
};

#endif // MAINWINDOW_H
