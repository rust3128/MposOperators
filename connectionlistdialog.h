#ifndef CONNECTIONLISTDIALOG_H
#define CONNECTIONLISTDIALOG_H

#include <QDialog>
#include <QSqlTableModel>
#include <QItemSelection>


namespace Ui {
class ConnectionListDialog;
}

class ConnectionListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionListDialog(int ID, QString name, QWidget *parent = 0);
    ~ConnectionListDialog();

private slots:
    void on_pushButtonCurrent_clicked();
    void userSelectionChanged(QItemSelection item);

    void on_pushButtonAdd_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonExit_clicked();

public:
    bool getChage();
private:
    void createUI();
private:
    Ui::ConnectionListDialog *ui;
    QSqlTableModel *model = new QSqlTableModel();
    int currentID;       //текщее подключение;
    QString currentName; //Имя текущего подключения
    int selectedID;
    QString selectedName;
    bool isChanged;
};

#endif // CONNECTIONLISTDIALOG_H
