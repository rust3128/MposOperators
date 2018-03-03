#include "usersdialog.h"
#include "ui_usersdialog.h"
#include "loggingcategories.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>


UsersDialog::UsersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UsersDialog)
{
    ui->setupUi(this);
    dblite = QSqlDatabase::database("lite");
    createUI();
}

UsersDialog::~UsersDialog()
{
    delete ui;
}

void UsersDialog::createUI()
{
    ui->frameMapper->hide();
    ui->lineEditIsAktive->hide();
    ui->lineEditIsAktive->setText("1");
    ui->pbEdit->setDisabled(true);
    ui->pbDelete->setDisabled(true);

    modelUsers = new QSqlTableModel(this,dblite);
    modelUsers->setTable("users");
//    qDebug() << modelUsers->lastError().text();
    modelUsers->setHeaderData(1,Qt::Horizontal,"Логин");
    modelUsers->setHeaderData(2,Qt::Horizontal,"Ф.И.О.");
    modelUsers->setHeaderData(3,Qt::Horizontal,"Пароль");
    modelUsers->select();
    modelUsers->setFilter("user_active=1");

    mapper = new QDataWidgetMapper();
    mapper->setModel(modelUsers);
    mapper->addMapping(ui->leLogin,1);
    mapper->addMapping(ui->leFIO,2);
    mapper->addMapping(ui->lePass,3);
    mapper->addMapping(ui->lineEditIsAktive,4);

    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);


    ui->tableView->setModel(modelUsers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(4);
    ui->tableView->hideColumn(3);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());

    connect(ui->tableView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,SLOT(userSelectionChanged(QItemSelection)));

}

void UsersDialog::manageData(int userID)
{
    if(userID == -1) {
        modelUsers->insertRow(modelUsers->rowCount(QModelIndex()));
        mapper->toLast();
    }
    else {
//        mapper->setCurrentModelIndex(modelUsers->index(row,0));
    }
}

void UsersDialog::on_pbNew_clicked()
{
    ui->frameMapper->show();
    ui->tableView->setDisabled(true);
    ui->pbNew->setDisabled(true);
    ui->pbEdit->setDisabled(true);
    ui->pbDelete->setDisabled(true);
    isNew=true;
    manageData(-1);
}

void UsersDialog::on_pbSave_clicked()
{
    QSqlQuery q = QSqlQuery(dblite);

    QString strSql=QString("SELECT EXISTS (SELECT user_name FROM users WHERE user_name='%1')")
            .arg(ui->leLogin->text().trimmed());
    q.prepare(strSql);
    q.exec();
    q.next();
    if(q.value(0) != 0 && isNew) {
           QMessageBox::information(this, "Ошибка имени пользователя",
                                    "Пользователь с таким логином уже существует");
    } else {
        mapper->submit();
        modelUsers->submitAll();
        ui->frameMapper->hide();
        ui->tableView->setDisabled(false);
        ui->pbNew->setDisabled(false);
        ui->pbEdit->setDisabled(false);
        ui->pbDelete->setDisabled(false);
        qInfo(logInfo()) << "Добавлен пользователь "+ui->leLogin->text();
    }

}

void UsersDialog::userSelectionChanged(QItemSelection selection)
{
    mapper->setCurrentModelIndex(selection.indexes().first());
    ui->pbEdit->setDisabled(false);
    ui->pbDelete->setDisabled(false);
}

void UsersDialog::on_pbEdit_clicked()
{
    ui->frameMapper->show();
    ui->tableView->setDisabled(true);
    ui->pbNew->setDisabled(true);
    ui->pbEdit->setDisabled(true);
    ui->pbDelete->setDisabled(true);
    isNew=false;
    manageData(1);
}

void UsersDialog::on_pbDelete_clicked()
{
    QSqlQuery q = QSqlQuery(dblite);
    QModelIndex idx = ui->tableView->selectionModel()->currentIndex();
    int userID = modelUsers->data(modelUsers->index(idx.row(),0)).toInt();
    QString strSQL = QString("UPDATE users SET user_active=0 WHERE user_id='%1'")
            .arg(userID);
    if(!q.exec(strSQL)) {
        qDebug(logDebug()) << "Не удалось деактивировать пользователя. Причина: "
                           << q.lastError().text();
        return;
    }
    qInfo(logInfo()) << "Пользователь " << modelUsers->data(modelUsers->index(idx.row(),1)).toString() << "деактивирован.";
    ui->pbEdit->setDisabled(true);
    ui->pbDelete->setDisabled(true);
    modelUsers->select();
    modelUsers->setFilter("user_active=1");
}

void UsersDialog::on_pbExit_clicked()
{
    this->reject();
}

void UsersDialog::on_pbCancel_clicked()
{
    ui->frameMapper->hide();
    ui->tableView->setDisabled(false);
    ui->pbNew->setDisabled(false);
    ui->pbEdit->setDisabled(false);
    ui->pbDelete->setDisabled(false);
}
