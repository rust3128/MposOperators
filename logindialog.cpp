#include "logindialog.h"
#include "ui_logindialog.h"
#include "loggingcategories.h"
#include <QSqlRecord>



LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    createUI();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::createUI()
{
    dblite = QSqlDatabase::database("lite");
    users = new QSqlQueryModel();
    users->setQuery("SELECT user_id, user_name, user_pass, user_fio FROM users", dblite);
    ui->comboBoxLogin->setModel(users);
    ui->comboBoxLogin->setModelColumn(1);
    ui->comboBoxLogin->setCurrentIndex(-1);
    currUserID=-1;
    ui->labelInfo->setText("");
}

void LoginDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    switch (ui->buttonBox->standardButton(button)) {
    case QDialogButtonBox::Ok:
        if(ui->comboBoxLogin->currentIndex()>=0) verifyLogin();
        break;
    case QDialogButtonBox::Cancel:
        this->reject();
        break;
    default:
        break;
    }
}

void LoginDialog::verifyLogin()
{
    if(userPass==ui->lineEditPassword->text().trimmed()) {
        qInfo(logInfo()) << QString("Пользователь: %1. Успешный вход в систему.").arg(ui->comboBoxLogin->currentText());
        this->accept();
    } else {
        ui->labelInfo->setText("Не верный пароль!");
        qWarning(logInfo()) << QString("Пользователь: %1. Не верный пароль!.").arg(ui->comboBoxLogin->currentText());
        ui->lineEditPassword->clear();
        return;
    }
}

void LoginDialog::on_comboBoxLogin_activated(int idx)
{
    userPass="";
    indexModel=users->index(idx,0,QModelIndex());

    currUserID=users->data(users->index(idx,0)).toInt();
    userPass=users->data(users->index(idx,2)).toString();
//    qDebug(logDebug()) << userPass;
}

void LoginDialog::on_lineEditPassword_textEdited()
{
    ui->labelInfo->clear();
}

QSqlRecord LoginDialog::getUser()
{
    return users->record(indexModel.row());
}
