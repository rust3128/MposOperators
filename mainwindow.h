#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QSqlRecord>
#include <QLabel>
#include <QSqlTableModel>
#include <QSqlQueryModel>

struct userdata
{
    int id;
    QString fio;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSqlRecord user, QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    userdata currentUser;
    QLabel *labelUser;
    QSqlQueryModel *modelTerminals;
private:
    void closeEvent(QCloseEvent *event);
    void infoUser2StatusBar();
    void openCentralDB(void);
    void createUI();
    void setupTerminalModel();


};

#endif // MAINWINDOW_H
