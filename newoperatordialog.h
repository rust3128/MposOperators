#ifndef NEWOPERATORDIALOG_H
#define NEWOPERATORDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class NewOperatorDialog;
}

class NewOperatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewOperatorDialog(QWidget *parent = 0);
    ~NewOperatorDialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::NewOperatorDialog *ui;
};

#endif // NEWOPERATORDIALOG_H
