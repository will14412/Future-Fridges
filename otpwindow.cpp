#include "otpwindow.h"
#include "chefswindow.h"
#include "database.h"
#include "ddwindow.h"
#include "ui_otpwindow.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRandomGenerator>
#include <QTime>

OTPWindow::OTPWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OTPWindow)
{
    ui->setupUi(this);
}

OTPWindow::~OTPWindow()
{
    delete ui;
}

void OTPWindow::on_RequestOTPButton_clicked()
{
    /* Sends an email to the delivery driver containing
    the OTP. Also saves this OTP to be checked later. */
    // Seed the random number generator

    QString userEmail = ui->EmailLineEdit->text();
    if (!(selectDriver(userEmail)))
    {
        QMessageBox::warning(this, "Warning", "This email does not have permissions");
        return;
    }

    QRandomGenerator randomGenerator(static_cast<quint64>(QTime::currentTime().msec()));

    int randomValue = randomGenerator.bounded(1000001);
    qDebug() << randomValue;
    OTP = randomValue;

    if(sendEmail(userEmail, OTP)){

        QMessageBox::information(this, "Success", "The OTP has been successfully sent!");
    }else{

        QMessageBox::critical(this, "Fail", "Email failed to send check console logs");
    }
}

bool OTPWindow::sendEmail(QString Email, int OTP){

    QString textOTP = QString::number(OTP);

    qDebug() << Email;
    qDebug() << textOTP;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    //local server 2 options docker or node.js
    //docker pull cianely8/emailapp:v1.0
    //docker run -p 3000:3000 cianely8/emailapp:v1.0

    QUrl url("http://localhost:3000/send-email");

    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["to"] = Email;//users email address
    json["subject"] = "Your one-time password!";
    json["text"] = textOTP;

    QJsonDocument jsonData(json);

    QByteArray postData = jsonData.toJson();

    QNetworkReply *reply = manager->post(request, postData);

    QEventLoop loop;
    QObject::connect(manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Request succeeded!";
        qDebug() << "Response:" << reply->readAll(); // Read the response data
        reply->deleteLater();
        manager->deleteLater();
        return true;
    } else {
        qDebug() << "Request failed:" << reply->errorString();
        reply->deleteLater();
        manager->deleteLater();
        return false;
    }
}

void OTPWindow::on_ChefSwitchButton_clicked()
{
    ChefsWindow *chefsWindow = new ChefsWindow();
    this->hide();
    chefsWindow->show();
}

void OTPWindow::on_OTPButton_clicked()
{
    // Get the entered OTP value from the OTPLineEdit
    int enteredOTP = (ui->OTPLineEdit->text()).toInt();

    if (enteredOTP == 0)
    {
        QMessageBox::warning(this, "Invalid OTP", "The entered OTP is not valid. Please try again.");
        ui->OTPLineEdit->clear();
        return;
    }

    // Check if the entered OTP exists in the active column of the database
    if (isOTPValid(enteredOTP))
    {
        // Reset OTP
        OTP = 0;

        // OTP is valid, proceed to the next window (DDWindow)
        DDWindow *ddWindow = new DDWindow();
        this->hide();
        ddWindow->show();

    } else
    {
        // OTP is not valid, show an error message or take appropriate action
        QMessageBox::warning(this, "Invalid OTP", "The entered OTP is not valid. Please try again.");
        ui->OTPLineEdit->clear(); // Clear the OTPLineEdit for a new attempt
        return;
    }
}

bool OTPWindow::isOTPValid(const int& enteredOTP)
{
    if (enteredOTP == OTP)
    {
        return true;
    }

    return false; // Return false if the OTP is not found or there is an error
}
