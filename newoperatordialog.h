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
    QStringList getNewOper();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::NewOperatorDialog *ui;
    bool veryfiData();
    QStringList operData;
};

#endif // NEWOPERATORDIALOG_H
