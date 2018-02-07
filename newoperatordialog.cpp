#include "newoperatordialog.h"
#include "ui_newoperatordialog.h"
#include <QDebug>
#include <QMessageBox>

NewOperatorDialog::NewOperatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewOperatorDialog)
{
    ui->setupUi(this);

}

NewOperatorDialog::~NewOperatorDialog()
{
    delete ui;
}

QStringList NewOperatorDialog::getNewOper()
{
    return operData;
}

void NewOperatorDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    switch (ui->buttonBox->standardButton(button)) {
    case QDialogButtonBox::Save:
        if(veryfiData()){
            operData << ui->lineEditLogin->text().trimmed() << ui->lineEditFIO->text().trimmed() << ui->lineEditPass->text().trimmed();
            this->accept();
        }
        break;
    default:
        this->reject();
        break;
    }
}

bool NewOperatorDialog::veryfiData()
{

    if(ui->lineEditLogin->text().length()==0) {
        QMessageBox::information(this,"Ошибка","Не указан логин пользователя!",QMessageBox::Ok);
        return false;
    }
    if(ui->lineEditFIO->text().length()==0) {
        QMessageBox::information(this,"Ошибка","Не указаны ФИО пользователя!",QMessageBox::Ok);
        return false;
    }
    if(ui->lineEditPass->text().length()==0) {
        QMessageBox::information(this,"Ошибка","Не указан пароль пользователя!",QMessageBox::Ok);
        return false;
    }
    return true;
}
