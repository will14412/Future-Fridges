#include "mainwindow.h"

#include "chefswindow.h"
#include "adminactions.h"
#include "signupdialog.h"
#include "./ui_mainwindow.h"
#include "chefswindow.h"
#include "database.h"


using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_SignUpLabel_clicked()
{
    SignUpDialog* signUpDialog = new SignUpDialog();
    this->hide();
    signUpDialog->show();
}

void MainWindow::on_CancelButton_clicked()
{
    ChefsWindow *chefsWindow = new ChefsWindow();
    this->hide();
    chefsWindow->show();
}

void MainWindow::on_LoginButton_clicked()
{

    QString username = ui->UsernameLineEdit->text();
    QString password = ui->PasswordLineEdit->text();

    bool valid = login(username, password);
    qDebug() << "Username: " << username;
    qDebug() << "Password: " << password;

    if (!valid)
    {
        ui->ErrorLabel->setText("Invalid Username / Password!");
        return;
    }

    adminactions* adminActions = new adminactions(); // some naming inconsistencies (adminactions, ChefsWindow etc.)
    this->hide();
    adminActions->show();
}
