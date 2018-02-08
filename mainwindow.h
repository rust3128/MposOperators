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
#include "operatorsdata.h"


struct userdata
{
    int id;
    QString fio;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSqlRecord user, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionUsers_triggered();
    void on_tableView_doubleClicked(const QModelIndex &idx);
    void startDBConnect();
    void finishDBConnect();
    void errogConnectInfo(QString str);
    void getStaus(bool status);
    void getTableOperators(QVector<dataOp> tblOp);
    void filterSet();
    void on_pushButton_clicked();
    void on_pushButtonApplay_clicked();

    void on_pushButtonActive_clicked();

    void on_tableWidget_itemClicked(QTableWidgetItem *item);

private:
    Ui::MainWindow *ui;
    userdata currentUser;
    QLabel *labelUser;
    QSqlQueryModel *modelTerminals;
private:
    void closeEvent(QCloseEvent *event);
    void infoUser2StatusBar();
    void openCentralDB(void);
    void createUI();
    void setupTerminalModel();
    void createUIOperarors();
    QMap<QString, QString> azsConn;
    QProgressDialog *progress;
    bool isConnected;
    QThread *thread;
    QModelIndex idxTerm;
    dataOp s;
    QVector<dataOp> opVector;
    QStringList insOp;
    QVector <QStringList> listChange;


};

#endif // MAINWINDOW_H
