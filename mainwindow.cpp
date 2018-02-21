#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loggingcategories.h"
#include "usersdialog.h"
#include "dbaseconnect.h"
#include "newoperatordialog.h"
#include "operatorchanged.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QMetaType>

MainWindow::MainWindow(QSqlRecord user, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1024,768);
    currentUser.id=user.value("user_id").toInt();
    currentUser.fio=user.value("user_fio").toString();
    ui->frameOperators->hide();
    ui->labelMessageWork->hide();
    listChange.clear();


    infoUser2StatusBar();
    openCentralDB();
    setupTerminalModel();

    connect(ui->radioButtonActive,SIGNAL(clicked()),this,SLOT(filterSet()));
    connect(ui->radioButtonAll,SIGNAL(clicked()),this,SLOT(filterSet()));
    connect(ui->radioButtonRemove,SIGNAL(clicked()),this,SLOT(filterSet()));

    ui->tableWidget->setIconSize(QSize(32,32));
    ui->pushButtonActive->setIconSize(QSize(32,32));
    enabledApplay();

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
    int currIdConn;
    QSqlDatabase dblite = QSqlDatabase::database("lite");
    QSqlDatabase fbcentral = QSqlDatabase::addDatabase("QIBASE","central");
    QSqlQuery q = QSqlQuery(dblite);

    QString strSQL = "SELECT currentID FROM usedconn";
    q.exec();
    q.next();
    currIdConn=q.value(0).toInt();

    strSQL = "SELECT * FROM connections WHERE conn_id=3";
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
    ui->tableWidget->clear();
    opVector.clear();
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
    connect(dbConn,&DBaseConnect::sendCurrentOperators,this,&MainWindow::getCurrentOperators,Qt::DirectConnection);
    connect(dbConn,SIGNAL(sendStatus(bool)),this,SLOT(getStaus(bool)),Qt::DirectConnection);
    connect(dbConn,SIGNAL(connectionError(QString)),this,SLOT(errogConnectInfo(QString)),Qt::DirectConnection);

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
}

void MainWindow::getCurrentOperators(int curID)
{
    currentOperator=curID;
}



void MainWindow::filterSet()
{
    bool match, isSelected=false;
    int firsrRow = 0;
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
            if(match && !isSelected){
                firsrRow=i;
                isSelected=true;
            }
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
            if(match && !isSelected){
                firsrRow=i;
                isSelected=true;
            }
        }
    }
    ui->tableWidget->selectRow(firsrRow);


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

        if(opVector.at(i).isactive){
            ui->tableWidget->setItem(i,4,new QTableWidgetItem("1"));
            ui->tableWidget->setItem(i,5,new QTableWidgetItem(icoWork,"Работает",1));
        } else {

            ui->tableWidget->setItem(i,4,new QTableWidgetItem("0"));
            ui->tableWidget->setItem(i,5,new QTableWidgetItem(icoDel,"Уволен",1));
        }
        if(opVector.at(i).Id==currentOperator){
            ui->tableWidget->item(i,1)->setBackground(Qt::green);
            ui->tableWidget->item(i,2)->setBackground(Qt::green);
            ui->tableWidget->item(i,3)->setBackground(Qt::green);
            ui->tableWidget->item(i,5)->setBackground(Qt::green);
        }
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
        //Добавляем запись в Виджет
        int rowNew = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(rowNew);

//        ui->tableWidget->setItem(i,0,new QTableWidgetItem(opVector.at(i).Id));
        ui->tableWidget->setItem(rowNew,1,new QTableWidgetItem(insOp.at(0)));
        ui->tableWidget->setItem(rowNew,2,new QTableWidgetItem(insOp.at(1)));
        ui->tableWidget->setItem(rowNew,3,new QTableWidgetItem(insOp.at(2)));
        ui->tableWidget->setItem(rowNew,4,new QTableWidgetItem("1"));
        ui->tableWidget->setItem(rowNew,5,new QTableWidgetItem(icoWork,"Работает",1));

        ui->tableWidget->item(item->row(),1)->setBackground(Qt::red);
        ui->tableWidget->item(item->row(),2)->setBackground(Qt::red);
        ui->tableWidget->item(item->row(),3)->setBackground(Qt::red);
        ui->tableWidget->item(item->row(),5)->setBackground(Qt::red);

        filterSet();

//        qDebug() << "Пробуем сохранять" << listChange;
        enabledApplay();
        return;
    }
    if(dialogcode == QDialog::Rejected) {
        enabledApplay();
        return;
    }

}

void MainWindow::on_pushButtonApplay_clicked()
{
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

        if (msgBox.exec() == QMessageBox::AcceptRole){
            applayAzs();
        }
        else {
            listChange.clear();
        }

}

void MainWindow::on_pushButtonActive_clicked()
{

    if(item->row() >= opVector.size()){
        QMessageBox::information(this,"Внимание","Данные о сотруднике еще не внесены в базу данных АЗС.<br>Увольнение не возможно.");
        return;
    }
    QString strSQL, strNote;
    QStringList list;
    if(ui->pushButtonActive->text()=="Уволить"){
        strNote = QString("Увольнение оператора.<br>Логин: <b>%1</b>, ФИО: <b>%2</b>.")
                        .arg(opVector.at(item->row()).login)
                        .arg(opVector.at(item->row()).fio);
        strSQL = QString("UPDATE operators SET isactive='F' WHERE operator_id=%1")
                        .arg(opVector.at(item->row()).Id);
        ui->tableWidget->item(item->row(),1)->setBackground(Qt::red);
        ui->tableWidget->item(item->row(),2)->setBackground(Qt::red);
        ui->tableWidget->item(item->row(),3)->setBackground(Qt::red);
        ui->tableWidget->item(item->row(),5)->setBackground(Qt::red);
    } else {
        strNote = QString("Активация оператора.<br>Логин: <b>%1</b>, ФИО: <b>%2</b>.")
                        .arg(opVector.at(item->row()).login)
                        .arg(opVector.at(item->row()).fio);
        strSQL = QString("UPDATE operators SET isactive='T' WHERE operator_id=%1")
                        .arg(opVector.at(item->row()).Id);
        ui->tableWidget->item(item->row(),1)->setBackground(Qt::red);
        ui->tableWidget->item(item->row(),2)->setBackground(Qt::red);
        ui->tableWidget->item(item->row(),3)->setBackground(Qt::red);
        ui->tableWidget->item(item->row(),5)->setBackground(Qt::red);
    }

    list << strNote << strSQL;
    listChange.push_back(list);
    ui->pushButtonActive->setDisabled(true);
    enabledApplay();
}


void MainWindow::on_tableWidget_itemSelectionChanged()
{
    item = ui->tableWidget->currentItem();
    if(item->row() >= opVector.size()){
        return;
    }
    if(item->backgroundColor() == Qt::red){
        ui->pushButtonActive->setDisabled(true);
        return;
    } else {
        ui->pushButtonActive->setDisabled(false);
    }
    ui->labelMessageWork->hide();

    if(opVector.at(item->row()).Id == currentOperator){
        ui->labelMessageWork->show();
        ui->labelMessageWork->setText("Оператор "+opVector.at(item->row()).fio+" работает в текущей смене. Измененния не возможны!");
        ui->pushButtonActive->setDisabled(true);
        return;
    }
    ui->pushButtonActive->setDisabled(false);

//    qDebug() << "Row"<<item->row() << "Меняем. ID:" << opVector.at(item->row()).Id << opVector.at(item->row()).fio;
    if(opVector.at(item->row()).isactive) {
        ui->pushButtonActive->setText("Уволить");
        ui->pushButtonActive->setIcon(icoDel);
    } else {
        ui->pushButtonActive->setText("Принять");
        ui->pushButtonActive->setIcon(icoWork);
    }
}

void MainWindow::applayAzs()
{
    OperatorChanged *opCh = new OperatorChanged();
    modify = new QThread;
    qRegisterMetaType<sqlList>("qslList");

    progress = new QProgressDialog();
    progress->setWindowModality(Qt::WindowModal);
    progress->setLabelText("Выполнение изменений в базе данных АЗС\n"+ui->labelAzsTitle->text());
    progress->setCancelButton(0);
    progress->setRange(0,0);
    progress->setMinimumDuration(0);

    connect(modify,&QThread::started,this,&MainWindow::transacionStart);
    connect(modify,&QThread::started,opCh,&OperatorChanged::sqlExecute);
    connect(modify,&QThread::finished,this,&MainWindow::finishChange);

    connect(this,&MainWindow::sendConnName,opCh,&OperatorChanged::getConnName,Qt::DirectConnection);
    connect(this,&MainWindow::sendChangeList,opCh,&OperatorChanged::getListSql,Qt::DirectConnection);

    connect(opCh,&OperatorChanged::finish,modify,&QThread::terminate,Qt::DirectConnection);

    emit sendConnName(azsConn.value("conName"));
    emit sendChangeList(listChange);

    modify->start();
    opCh->moveToThread(modify);
}

void MainWindow::transacionStart()
{
    progress->show();

}

void MainWindow::finishChange()
{
    progress->cancel();
    otherAzs();
}

void MainWindow::on_pushButtonOtherAzs_clicked()
{
    if(listChange.size()>0) {
        QString messStr = QString("<p><strong>Остались не выполнены следующие изменения:</strong></p>"
                                  "<ol>");
        for(int i=0;i<listChange.size();++i){
            messStr+="<li>"+listChange.at(i).at(0)+"</li>";
        }
        messStr+="</ol>";

        messStr+="<strong>Пожалуйста подтвердите следующие действия.</strong>";

        QMessageBox msgBox(QMessageBox::Warning,
                           QString::fromUtf8("Не сохраненные изменения"),
                           messStr,
                           0, this);
        msgBox.addButton(QString::fromUtf8("&Применить изменения"),
                         QMessageBox::AcceptRole);
        msgBox.addButton(QString::fromUtf8("&Отменить изменения"),
                         QMessageBox::RejectRole);

        if (msgBox.exec() == QMessageBox::AcceptRole){
            qDebug() << "Будем сохранять";
            applayAzs();
        }
        else {
            listChange.clear();
            otherAzs();
        }
    }
}

void MainWindow::enabledApplay()
{
    if(listChange.size()>0) {
        ui->pushButtonApplay->setEnabled(true);
    } else {
        ui->pushButtonApplay->setEnabled(false);
    }
}

void MainWindow::otherAzs()
{
    // Remove all items
    ui->tableWidget->clearContents();

    // Set row count to 0 (remove rows)
    ui->tableWidget->setRowCount(0);

    ui->frameOperators->hide();
    ui->tableView->show();
    ui->labelSelect->show();
    listChange.clear();
}
