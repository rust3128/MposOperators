#ifndef USERSDIALOG_H
#define USERSDIALOG_H

#include <QDialog>
#include <QSqlTableModel>
#include <QDataWidgetMapper>
#include <QItemSelection>

namespace Ui {
class UsersDialog;
}

class UsersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsersDialog(QWidget *parent = 0);
    ~UsersDialog();

private slots:
    void on_pbNew_clicked();
    void on_pbSave_clicked();
    void userSelectionChanged(QItemSelection selection);

    void on_pbEdit_clicked();

    void on_pbDelete_clicked();

    void on_pbExit_clicked();

    void on_pbCancel_clicked();

private:
    void createUI();
    void manageData(int userID);
    Ui::UsersDialog *ui;
    QSqlTableModel *modelUsers;
    QSqlDatabase dblite;
    QDataWidgetMapper *mapper;
    bool isNew;
};

#endif // USERSDIALOG_H
