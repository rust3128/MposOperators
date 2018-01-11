#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loggingcategories.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>



#include <QMessageBox>

MainWindow::MainWindow(QSqlRecord user, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1024,768);
    currentUser.id=user.value("user_id").toInt();
    currentUser.fio=user.value("user_fio").toString();

    infoUser2StatusBar();
    openCentralDB();
    setupTerminalModel();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    if (QMessageBox::Yes == QMessageBox::question(this, "Закрыть?",
                              "Уверены?",
                              QMessageBox::Yes|QMessageBox::No))
        {
            qInfo(logInfo()) << "Завершение работы программы.";
            event->accept();
        }
}

void MainWindow::infoUser2StatusBar()
{
    labelUser = new QLabel(this);
    labelUser->setText("Пользователь: "+currentUser.fio);
    ui->statusBar->addPermanentWidget(labelUser);
}

void MainWindow::openCentralDB()
{
    QSqlDatabase dblite = QSqlDatabase::database("lite");
    QSqlDatabase fbcentral = QSqlDatabase::addDatabase("QIBASE","central");
    QSqlQuery q = QSqlQuery(dblite);
    QString strSQL = "SELECT * FROM connections WHERE conn_id=1";
    q.exec(strSQL);
    q.next();

    fbcentral.setHostName(q.value("conn_host").toString());
    fbcentral.setDatabaseName(q.value("conn_db").toString());
    fbcentral.setUserName(q.value("conn_user").toString());
    fbcentral.setPassword(q.value("conn_pass").toString());

    if(!fbcentral.open()) {
        qCritical(logCritical()) << "Не возможно подключится к центральной базе данных." << fbcentral.lastError().text();
        QMessageBox::critical(0,"Ошибка подключения", QString("Не возможно открыть базу данных!\n%1\nПроверьте настройку подключения.")
                .arg(fbcentral.lastError().text()),
                QMessageBox::Ok);
    }


}

void MainWindow::setupTerminalModel()
{
    QSqlDatabase fbcentral = QSqlDatabase::database("central");
    modelTerminals = new QSqlQueryModel();
    QString strSQL = "SELECT t.TERMINAL_ID, TRIM(t.name) AS NAME, t.PHONE, c.SERVER_NAME, c.DB_NAME, c.CON_PASSWORD FROM terminals t "
            "LEFT JOIN CONNECTIONS c ON t.TERMINAL_ID=c.TERMINAL_ID "
            "WHERE t.TERMINALTYPE=3 AND t.ISACTIVE='T' AND c.CONNECT_ID=2 "
            "ORDER BY t.TERMINAL_ID";
    modelTerminals->setQuery(strSQL,fbcentral);
    modelTerminals->setHeaderData(0,Qt::Horizontal,tr("АЗС"));
    modelTerminals->setHeaderData(1,Qt::Horizontal,tr("Адрес"));
    modelTerminals->setHeaderData(2,Qt::Horizontal,tr("Телефон"));

    ui->tableView->setModel(modelTerminals);
    for(int i=3; i<6; ++i) {
        ui->tableView->hideColumn(i);
    }
    ui->tableView->verticalHeader()->hide();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());


}




