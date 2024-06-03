#include "signupdialog.h"
#include "mainwindow.h"
#include "ui_signupdialog.h"
#include "database.h"

SignUpDialog::SignUpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignUpDialog)
{
    ui->setupUi(this);
}

SignUpDialog::~SignUpDialog()
{
    delete ui;
}

void SignUpDialog::on_submit_clicked()
{
    /* Creates a new user and saves thier details. Whoever
    is working on this can include additonal information if
    they feel it is needed */

    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QString confirmPassword = ui->confirmLineEdit->text();

    if (!(password == confirmPassword))
    {
        ui->errorLabel->setText("Password does not match!");
        return;
    }

    if (!(signup(username, password)))
    {
        ui->errorLabel->setText("Username already in use!");
        return;
    }

    if (username.isEmpty() || password.isEmpty())
    {
        ui->errorLabel->setText("Cannot have a blank username or password!");
        return;
    }

    MainWindow* mainWindow = new MainWindow();
    this->hide();
    mainWindow->show();
}


void SignUpDialog::on_cancel_clicked()
{
    MainWindow* mainWindow = new MainWindow();
    this->hide();
    mainWindow->show();
}

