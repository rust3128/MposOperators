#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QAbstractButton>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_comboBoxLogin_activated(int idx);

    void on_lineEditPassword_textEdited();
public:
    QSqlRecord getUser();
private:
    Ui::LoginDialog *ui;
    QSqlQueryModel *users;
    QSqlDatabase dblite;
    int currUserID;
    QString userPass;
    QModelIndex indexModel;
    void createUI();
    void verifyLogin();

};

#endif // LOGINDIALOG_H
