#ifndef APPLAYDIALOG_H
#define APPLAYDIALOG_H

#include <QDialog>

namespace Ui {
class ApplayDialog;
}

class ApplayDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ApplayDialog(QWidget *parent = 0);
    ~ApplayDialog();

private:
    Ui::ApplayDialog *ui;
};

#endif // APPLAYDIALOG_H
