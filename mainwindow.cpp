#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loggingcategories.h"
#include "usersdialog.h"
#include "dbaseconnect.h"
#include "newoperatordialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

#include <QCheckBox>
#include <QHBoxLayout>




#include <QMessageBox>

MainWindow::MainWindow(QSqlRecord user, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1024,768);
    currentUser.id=user.value("user_id").toInt();
    currentUser.fio=user.value("user_fio").toString();
    ui->frameOperators->hide();
    listChange.clear();

    infoUser2StatusBar();
    openCentralDB();
    setupTerminalModel();

    connect(ui->radioButtonActive,SIGNAL(clicked()),this,SLOT(filterSet()));
    connect(ui->radioButtonAll,SIGNAL(clicked()),this,SLOT(filterSet()));
    connect(ui->radioButtonRemove,SIGNAL(clicked()),this,SLOT(filterSet()));

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
    QString strSQL = "SELECT * FROM connections WHERE conn_id=2";
    q.exec(strSQL);
    q.next();

    fbcentral.setHostName(q.value("conn_host").toString());
    fbcentral.setDatabaseName(q.value("conn_db").toString());
    fbcentral.setUserName(q.value("conn_user").toString());
    fbcentral.setPassword(q.value("conn_pass").toString());

    if(!fbcentral.open()) {
        qCritical(logCritical()) << "Не возможно подключится к центральной базе данных" << fbcentral.lastError().text();
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



void MainWindow::on_actionUsers_triggered()
{
    UsersDialog *usersDlg = new UsersDialog();
    usersDlg->exec();
}



void MainWindow::on_tableView_doubleClicked(const QModelIndex &idx)
{
    QString pass;
    listChange.clear();
    idxTerm = idx;
    azsConn.insert("conName","azs"+modelTerminals->data(modelTerminals->index(idx.row(),0)).toString());
    azsConn.insert("server",modelTerminals->data(modelTerminals->index(idx.row(),3)).toString());
    azsConn.insert("basename",modelTerminals->data(modelTerminals->index(idx.row(),4)).toString());
    azsConn.insert("login","SYSDBA");
    if(modelTerminals->data(modelTerminals->index(idx.row(),5)).toString() == "juoesj8=")
        pass="island15";
    if(modelTerminals->data(modelTerminals->index(idx.row(),5)).toString() == "ncvxjxrm‚")
        pass="masterkey";
    if(modelTerminals->data(modelTerminals->index(idx.row(),5)).toString() == "twqsnrhlv")
        pass="sunoiladm";
    azsConn.insert("password",pass);

    DBaseConnect *dbConn = new DBaseConnect(azsConn);
    thread = new QThread;

    progress = new QProgressDialog();
    progress->setWindowModality(Qt::WindowModal);
    progress->setLabelText("Подключение к базе данных\n"+modelTerminals->data(modelTerminals->index(idx.row(),1)).toString());
    progress->setCancelButton(0);
    progress->setRange(0,0);
    progress->setMinimumDuration(0);

    connect(thread,SIGNAL(started()),this,SLOT(startDBConnect()));
    connect(thread,SIGNAL(started()),dbConn,SLOT(createConnection()));
    connect(thread,SIGNAL(finished()),this,SLOT(finishDBConnect()));

    connect(dbConn,SIGNAL(fin()),thread,SLOT(terminate()),Qt::DirectConnection);
    connect(dbConn,SIGNAL(sendOperators(QVector<dataOp>)),this,SLOT(getTableOperators(QVector<dataOp>)),Qt::DirectConnection);
    connect(dbConn,SIGNAL(sendStatus(bool)),this,SLOT(getStaus(bool)));
    connect(dbConn,SIGNAL(connectionError(QString)),this,SLOT(errogConnectInfo(QString)));

    thread->start();
    dbConn->moveToThread(thread);

}

void MainWindow::startDBConnect()
{
    progress->show();
}

void MainWindow::finishDBConnect()
{
    progress->cancel();
    if(isConnected){
        qInfo(logInfo()) << "Подключились к терминалу " << modelTerminals->data(modelTerminals->index(idxTerm.row(),0)).toString();
    } else {
        qInfo(logInfo()) << "Не возможно подключится к терминалу" << modelTerminals->data(modelTerminals->index(idxTerm.row(),0)).toString();
        return;
    }
    ui->frameOperators->show();
    ui->tableView->hide();
    ui->labelSelect->hide();
    ui->tableWidget->clearContents();
    ui->labelAzsTitle->setText("АЗС "+modelTerminals->data(modelTerminals->index(idxTerm.row(),0)).toString()
                               +"\n"+modelTerminals->data(modelTerminals->index(idxTerm.row(),1)).toString());
    createUIOperarors();

}

void MainWindow::errogConnectInfo(QString str)
{
    QMessageBox::critical(this, qApp->tr("Ошибка подключения"),
                                  QString("Не могу установить соединение с базой данных АЗС!\nПричина: %1\nПроверьте наличие Интеренет подключения к АЗС.").arg(str),
                          QMessageBox::Ok);
}

void MainWindow::getStaus(bool status)
{
    isConnected=status;
}

void MainWindow::getTableOperators(QVector<dataOp> tblOp)
{
    opVector = tblOp;
    qDebug() << opVector.size();


}

void MainWindow::filterSet()
{
    bool match;
    if (ui->radioButtonAll->isChecked()){
        for(int i=0; i < ui->tableWidget->rowCount(); ++i){
           ui->tableWidget->setRowHidden(i,false);
        }

    } else if (ui->radioButtonActive->isChecked()){
        for( int i = 0; i < ui->tableWidget->rowCount(); ++i )
        {
            match = false;
            QTableWidgetItem *item = ui->tableWidget->item( i, 4 );
            if( item->text()=="1")
            {
                match = true;
            }

            ui->tableWidget->setRowHidden( i, !match );
        }

    } else if (ui->radioButtonRemove) {
        for( int i = 0; i < ui->tableWidget->rowCount(); ++i )
        {
            match = false;
            QTableWidgetItem *item = ui->tableWidget->item( i, 4 );
            if( item->text()=="0")
            {
                match = true;
            }

            ui->tableWidget->setRowHidden( i, !match );
        }
    }
}

void MainWindow::createUIOperarors()
{


    QStringList headers = QStringList() << "ID" << "Логин" << "ФИО" << "Пароль" << "work" <<"Статус";

    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setShowGrid(true); // Включаем сетку
    // Разрешаем выделение только одного элемента
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    // Разрешаем выделение построчно
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем заголовки колонок
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // Растягиваем последнюю колонку на всё доступное пространство
//    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    // Скрываем колонку под номером 0
    ui->tableWidget->hideColumn(0);
    ui->tableWidget->hideColumn(4);
    for(int i=0;i<opVector.size();++i){
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(opVector.at(i).Id));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(opVector.at(i).login));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(opVector.at(i).fio));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(opVector.at(i).pswd));

        QWidget *checkBoxActiv = new QWidget();
//        QCheckBox *checkBox = new QCheckBox();
        QLabel *labelWork = new QLabel();

        QHBoxLayout *layoutCheckBox = new QHBoxLayout(checkBoxActiv);
        layoutCheckBox->addWidget(labelWork);
        layoutCheckBox->setContentsMargins(0,0,0,0);

        QString str;

        if(opVector.at(i).isactive){
            str="<img src=\":/Images/user_accept.png\"> Работает";
            labelWork->setStyleSheet("font-size: 12pt; color: green");
            ui->tableWidget->setItem(i,4,new QTableWidgetItem("1"));
        } else {
            str="<img src=\":/Images/user_removed.png\"> Уволен";
            labelWork->setStyleSheet("font-size: 12pt; color: red");
            ui->tableWidget->setItem(i,4,new QTableWidgetItem("0"));
        }
        labelWork->setTextFormat(Qt::RichText);
        labelWork->setText(str);
        ui->tableWidget->setCellWidget(i,5,checkBoxActiv);
    }
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(40);
    ui->radioButtonActive->setChecked(true);
    filterSet();
}

void MainWindow::on_pushButton_clicked()
{
    QString strSql, strNote;
    QStringList list;
    NewOperatorDialog *dlgOper = new NewOperatorDialog();
    int dialogcode = dlgOper->exec();
    if(dialogcode == QDialog::Accepted) {
        insOp = dlgOper->getNewOper();
        strNote = QString("Новый оператор Логин: <b>%1</b>, ФИО: <b>%2</b>.")
                .arg(insOp.at(0))
                .arg(insOp.at(1));
        strSql = QString("INSERT INTO operators (login,fio,pswd,isactive) VALUES('%1','%2','%3','T')")
                    .arg(insOp.at(0))
                    .arg(insOp.at(1))
                    .arg(insOp.at(2));
        list << strNote << strSql;
        listChange.push_back(list);
        qDebug() << "Пробуем сохранять" << listChange;
        return;
    }
    if(dialogcode == QDialog::Rejected) {
        qDebug() << "Ничего не делаем.";
        return;
    }

}

void MainWindow::on_pushButtonApplay_clicked()
{
    if(listChange.isEmpty()){
        QMessageBox::information(this,"Внимание!","По данной АЗС не производились изменения в справочнике оператора.");
        return;
    } else {

        QString messStr = QString("<p><strong>Будут выполнены следующие изменения:</strong></p>"
                                  "<ol>");
        for(int i=0;i<listChange.size();++i){
            messStr+="<li>"+listChange.at(i).at(0)+"</li>";
        }
        messStr+="</ol>";
        QMessageBox msgBox(QMessageBox::Warning,
                           QString::fromUtf8("Предупреждение"),
                           messStr,
                           0, this);
        msgBox.addButton(QString::fromUtf8("&Применить изменения"),
                         QMessageBox::AcceptRole);
        msgBox.addButton(QString::fromUtf8("&Отменить изменения"),
                         QMessageBox::RejectRole);

        if (msgBox.exec() == QMessageBox::AcceptRole)
            qDebug() << "Будем сохранять";
        else
            qDebug() << "Откатываемся";
    }
}

void MainWindow::on_pushButtonActive_clicked()
{

}

void MainWindow::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    qDebug() << item->row();
}
