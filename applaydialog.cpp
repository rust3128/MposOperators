#include "applaydialog.h"
#include "ui_applaydialog.h"

ApplayDialog::ApplayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplayDialog)
{
    ui->setupUi(this);
}

ApplayDialog::~ApplayDialog()
{
    delete ui;
}
