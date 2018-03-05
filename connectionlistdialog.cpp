#include "connectionlistdialog.h"
#include "ui_connectionlistdialog.h"
#include "loggingcategories.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>



ConnectionListDialog::ConnectionListDialog(int ID, QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionListDialog)
{
    ui->setupUi(this);
    currentID=ID;
    currentName=name;
    createUI();
    isChanged=false;
}

ConnectionListDialog::~ConnectionListDialog()
{
    delete ui;
}

void ConnectionListDialog::createUI()
{
    QSqlDatabase dblite = QSqlDatabase::database("lite");
    model = new QSqlTableModel(this,dblite);
    model->setTable("connections");
    model->select();

    model->setHeaderData(0,Qt::Horizontal,tr("ID"));
    model->setHeaderData(1,Qt::Horizontal,tr("Имя"));
    model->setHeaderData(2,Qt::Horizontal,tr("Хост"));
    model->setHeaderData(3,Qt::Horizontal,tr("Файл БД"));
    model->setHeaderData(4,Qt::Horizontal,tr("Пользователь"));
    model->setHeaderData(5,Qt::Horizontal,tr("Пароль"));


    ui->tableView->setModel(model);

    ui->tableView->verticalHeader()->hide();
    ui->tableView->hideColumn(0);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());

    ui->labelCurrent->setText("Текущее подключение: "+currentName);

    connect(ui->tableView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,&ConnectionListDialog::userSelectionChanged);
}

void ConnectionListDialog::on_pushButtonCurrent_clicked()
{
    QSqlDatabase dblite = QSqlDatabase::database("lite");
    QSqlQuery q = QSqlQuery(dblite);
    q.prepare("UPDATE `usedconn` SET `currentID`=:id");
    q.bindValue(":id",selectedID);
    if(!q.exec()){
        qCritical(logCritical()) << "Неудалось установить текущее соединение!" << q.lastError().text();
    } else {
        qInfo(logInfo()) << "Соединение "+selectedName+" установлено как текущее.";
        currentID=selectedID;
        currentName=selectedName;
        ui->labelCurrent->setText("Текущее подключение: "+currentName);
        ui->pushButtonCurrent->setEnabled(false);
        isChanged=true;
    }
}

void ConnectionListDialog::userSelectionChanged(QItemSelection item)
{
    QModelIndex idx =item.indexes().first();
    selectedID = model->data(model->index(idx.row(),0)).toInt();
    selectedName = model->data(model->index(idx.row(),1)).toString();
    if(selectedID==currentID){
        ui->pushButtonCurrent->setEnabled(false);
    } else {
        ui->pushButtonCurrent->setEnabled(true);
    }
}

void ConnectionListDialog::on_pushButtonAdd_clicked()
{
    QSqlDatabase dblite = QSqlDatabase::database("lite");
    QSqlQuery q = QSqlQuery(dblite);
    q.prepare("INSERT INTO `connections`(`conn_name`,`conn_host`,`conn_db`,`conn_user`,`conn_pass`) "
              "VALUES ('Новое соединение','','','','')");
    if(!q.exec()){
        qCritical(logCritical()) << "Не удалось добавить новую запись." << q.lastError().text();
    } else {
        qInfo(logInfo()) << "Добавлена новая -запись в таблицу соединений.";
        model->select();
    }
}

void ConnectionListDialog::on_pushButtonDelete_clicked()
{
    QMessageBox::StandardButton reply;
    QString str;
    QSqlDatabase dblite = QSqlDatabase::database("lite");
    QSqlQuery q = QSqlQuery(dblite);
    str = QString("Вы дейстивтельно хотите удалить подключение %1?")
            .arg(selectedName);
    reply = QMessageBox::question(this, QString::fromUtf8("Внимание"),
                          (str),
                          QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        q.prepare("DELETE FROM `connections` WHERE `conn_id`=:id");
        q.bindValue(":id",selectedID);
        if(!q.exec()){
            qCritical(logCritical()) << "Не удалось удалить запись." << q.lastError().text();
        } else {
            model->select();
            qInfo(logInfo()) << "Соединение "+selectedName+" удалено.";
        }
    }
}

bool ConnectionListDialog::getChage()
{
    return isChanged;
}

void ConnectionListDialog::on_pushButtonExit_clicked()
{
    this->accept();
}
