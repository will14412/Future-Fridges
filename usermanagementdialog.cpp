#include "usermanagementdialog.h"
#include "adminactions.h"
#include "qsqlquery.h"
#include "ui_usermanagementdialog.h"
#include "database.h"

#include <QMessageBox>

UserManagementDialog::UserManagementDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserManagementDialog)
{
    ui->setupUi(this);
}

UserManagementDialog::~UserManagementDialog()
{
    delete ui;
}

void UserManagementDialog::on_RemoveButton_clicked()
{
    QString userToDelete = ui->UsernameLineEdit->text();

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :username");
    query.bindValue(":username", userToDelete);
    query.exec();

    if (query.next())
    {
        // User exists, proceed with deletion
        QSqlQuery deleteQuery;
        deleteQuery.prepare("DELETE FROM users WHERE username = :username");
        deleteQuery.bindValue(":username", userToDelete);
        if (deleteQuery.exec())
        {
            // Deletion successful
            QMessageBox::information(this, "Success", "User successfully removed.");
        } else
        {
            // Deletion failed
            QMessageBox::critical(this, "Error", "Failed to remove user.");
            return;
        }
    } else
    {
        // User does not exist
        QMessageBox::warning(this, "Warning", "User not found.");
        return;
    }

    adminactions* adminActions = new adminactions();
    this->hide();
    adminActions->show();
}

void UserManagementDialog::on_CancelButton_clicked()
{
    adminactions* adminActions = new adminactions();
    this->hide();
    adminActions->show();
}

void UserManagementDialog::on_SubmitNewDriverButton_clicked()
{
    QString driverEmail = ui->NewDriverLineEdit->text();

    if (selectDriver(driverEmail))
    {
        qDebug() << driverEmail;
        QMessageBox::warning(this, "Warning", "Driver already exists.");
        return;
    }

    if (driverEmail.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Please enter a valid email.");
        return;
    }

    if (!(insertDriver(driverEmail)))
    {
        QMessageBox::warning(this, "Warning", "New driver cannot be added.");
        return;
    }

    QMessageBox::information(this, "Success", "Succesfully added new driver!");
    adminactions* adminActions = new adminactions();
    this->hide();
    adminActions->show();
}

void UserManagementDialog::on_RemoveDriverButton_clicked()
{
    QString driverEmail = ui->DriverLineEdit->text();

    if (!(selectDriver(driverEmail)))
    {
        QMessageBox::warning(this, "Warning", "Driver doesn't exist.");
        return;
    }

    if (!(removeDriver(driverEmail)))
    {
        QMessageBox::warning(this, "Warning", "Driver cannot be removed.");
        return;
    }

    QMessageBox::information(this, "Success", "Succesfully removed driver!");
    adminactions* adminActions = new adminactions();
    this->hide();
    adminActions->show();
}

