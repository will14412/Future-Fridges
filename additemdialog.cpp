#include "additemdialog.h"
#include "chefswindow.h"
#include "ui_additemdialog.h"
#include "database.h"

#include <QDate>
#include <QMessageBox>
#include <qsqldatabase.h>

AddItemDialog::AddItemDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddItemDialog)
{
    ui->setupUi(this);
}

AddItemDialog::~AddItemDialog()
{
    delete ui;
}

void AddItemDialog::on_submit_clicked()
{
    QString name = ui->stockLineEdit->text();
    QString count = ui->amountSpinBox->text();
    QString metric = ui->comboBoxMetrics->currentText();
    QDate date = QDate::currentDate().addDays(ui->dateSpinBox->value());

    int convertToInt = count.toInt();

    if (name.isEmpty())
    {
        QMessageBox::warning(this, "Naming Error", "Please enter a name for the item.");
        return;
    }

    if (!addStock(name,convertToInt,metric,date))
    {
        QMessageBox::warning(this, "Add Item Error", "Failed to add item.");
        return;
    }

    close();
}

void AddItemDialog::on_cancel_clicked()
{
    ChefsWindow* chefsWindow = new ChefsWindow();
    this->hide();
    chefsWindow->show();
}

