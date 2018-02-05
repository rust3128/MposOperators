#include "newoperatordialog.h"
#include "ui_newoperatordialog.h"
#include <QDebug>

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

void NewOperatorDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    switch (ui->buttonBox->standardButton(button)) {
    case QDialogButtonBox::Save:
        break;
    default:
        this->reject();
        break;
    }
}
