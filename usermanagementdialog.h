#ifndef USERMANAGEMENTDIALOG_H
#define USERMANAGEMENTDIALOG_H

#include <QDialog>

namespace Ui {
class UserManagementDialog;
}

class UserManagementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserManagementDialog(QWidget *parent = nullptr);
    ~UserManagementDialog();

private slots:
    void on_RemoveButton_clicked();

    void on_CancelButton_clicked();

    void on_SubmitNewDriverButton_clicked();

    void on_RemoveDriverButton_clicked();

private:
    Ui::UserManagementDialog *ui;
};

#endif // USERMANAGEMENTDIALOG_H
