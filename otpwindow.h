#ifndef OTPWINDOW_H
#define OTPWINDOW_H

#include <QMainWindow>

namespace Ui {
class OTPWindow;
}

class OTPWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OTPWindow(QWidget *parent = nullptr);
    ~OTPWindow();

private slots:
    void on_ChefSwitchButton_clicked();

    void on_OTPButton_clicked();

    void on_RequestOTPButton_clicked();

    bool isOTPValid(const int& enteredOTP);

    bool sendEmail(QString Email, int OTP);



private:
    Ui::OTPWindow *ui;

    int OTP;
};

#endif // OTPWINDOW_H
