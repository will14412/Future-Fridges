#include "adminactions.h"
#include "chefswindow.h"
#include "database.h"
#include "ui_adminactions.h"
#include "usermanagementdialog.h"

#include <QCoreApplication>
#include <QDebug>
#include <QByteArray>
#include <QDesktopServices>
#include <QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDate>

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QMessageBox>
#include <QRandomGenerator>

adminactions::adminactions(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::adminactions)
{
    ui->setupUi(this);
    refreshFoodStockList();
}

adminactions::~adminactions()
{
    delete ui;
}

void adminactions::notifications()
{
    if ((checkExpired() == "") && (checkExpiring() == "") && (checkStock() == ""))
    {
        QMessageBox::information (this, "Notification", "There are no new notifications");
        return;
    }

    if (!(checkExpired() == ""))
    {
    QMessageBox::warning(this, "Warning", "The following products are expired: " + checkExpired());
    }

    if (!(checkExpiring() == ""))
    {
    QMessageBox::warning(this, "Warning", "The following products expire tomorrow: " + checkExpiring());
    }

    if (!(checkStock() == ""))
    {
    QMessageBox::warning(this, "Warning", "The following products are low on stock: " + checkStock());
    }
}

void adminactions::on_ReorderButton_clicked()
{
    // Check if an item is selected
    if (!validateSelectedItem())
    {
        return;
    } else
    {
        int amountToOrder = ui->AddAmountSpinBox->value();
        ui->ReceiptTextEdit->setPlainText(ui->ReceiptTextEdit->toPlainText() + "\n" + QString::number(amountToOrder) + " of " + selectedName);
    }
}

void adminactions::on_HealthSafetyButton_clicked()
{
    /* This forms the report for the health and safety inspector,
    maybe saves it as a text or pdf document and sent to the head
    chef via email */

    QSqlQuery query;
    query.exec("SELECT name, count, date FROM stock");

    // Open a text file for writing, clearing its contents if it already exists
    QFile file("HealthSafetyReport.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream out(&file);

        // Write column headers to the file
        out << "Name\tCount\tDate\tExpired\n";

        // Get the current system date
        QDate currentDate = QDate::currentDate();

        // Iterate through the query results and write to the file
        while (query.next())
        {
            QString name = query.value(0).toString();
            int count = query.value(1).toInt();
            QDate date = query.value(2).toDate();

            // Check if the date has expired
            bool isExpired = currentDate > date;

            // Write data to the file
            out << name << "\t" << count << "\t" << date.toString("yyyy-MM-dd") << "\t" << (isExpired ? "Yes" : "No") << "\n";
        }

        // Close the file
        file.close();

        // Inform the user that the report has been generated
        qDebug() << "Health and Safety report has been generated and saved to HealthSafetyReport.txt"; // change to dialog window
    } else
    {
        // Handle file opening error
        qDebug() << "Error opening file for writing.";
    }
}


void adminactions::on_LogoutButton_clicked()
{
    ChefsWindow* chefsWindow = new ChefsWindow();
    this->hide();
    chefsWindow->show();
}

void adminactions::refreshFoodStockList()
{
    // Clear existing items in the list widget
    ui->FoodStockList->clear();
    std::vector<std::vector<QString>> food_stock;

    if (!QSqlDatabase::database().isOpen())
    {
        qDebug() << "Database not open";
        return;
    }

    QSqlQuery query;

    //Query stock table
    if (!query.exec("SELECT name, count, metric, date FROM stock"))
    {
        qDebug() << "Error executing query:" << query.lastError().text();
        return;
    }

    //For each row in the database use pushback to add the item columns to an array and push the item to food stock array
    while (query.next())
    {
        std::vector<QString> item;
        item.push_back(query.value(0).toString()); // name
        item.push_back(query.value(1).toString()); // count
        item.push_back(query.value(2).toString()); // metric
        item.push_back(query.value(3).toString()); // date
        food_stock.push_back(item);
    }

    //Adds stock to list widget
    for (int i = 0; i < food_stock.size(); i++)
    {
        QListWidgetItem *newItem = new QListWidgetItem();
        newItem->setText(newItem->text() + "|    ");
        for (int j = 0; j < food_stock[i].size(); j++)
        {
            newItem->setText(newItem->text() + food_stock[i][j] + "    |    ");
        }
        ui->FoodStockList->addItem(newItem);
    }
}

void adminactions::on_FoodStockList_itemClicked(QListWidgetItem *item)
{
    if (item)
    {
        QString itemText = item->text();
        QStringList parts = itemText.split("|", Qt::SkipEmptyParts);
        if (parts.size() >= 2)
        {
            selectedName = parts[0].trimmed(); // Extract name
            selectedCount = parts[1].trimmed().toInt(); // Extract count

            // Now you have the extracted details, you can use them as needed
            qDebug() << "Selected Item= " << "Name:" << selectedName << ", Count:" << selectedCount;
        }
        ui->SelectedLabel->setText("Currently Selected: " + selectedName);
    }
}

bool adminactions::validateSelectedItem()
{
    if (selectedName.isEmpty() || selectedCount == 0)
    {
        QMessageBox::warning(this, "Action", "No item selected.");
        qDebug() << "No item selected";
        return false;
    }
    return true;
}


void adminactions::on_UserManagementButton_clicked()
{
    UserManagementDialog* userManagementDialog = new UserManagementDialog();
    this->hide();
    userManagementDialog->show();
}


void adminactions::on_SubmitButton_clicked()
{
    // Open a text file for writing, clearing its contents if it already exists
    QFile file("ReorderReceipt.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream out(&file);

        QDate currentDate = QDate::currentDate();

        // Write column headers to the file
        out << ui->ReceiptTextEdit->toPlainText() << "\n" << "Date Ordered: " << currentDate.toString("dd-MM-yyyy");

        // Close the file
        file.close();

        // Clear receipt
        ui->ReceiptTextEdit->setPlainText("Receipt:");

        // Inform the user that the report has been generated
        qDebug() << "New order reciept has been generated and saved to ReorderReceipt.txt"; // change to dialog window
    } else
    {
        // Handle file opening error
        qDebug() << "Error opening file for writing.";
    }
}

void adminactions::on_pushButton_clicked()
{
    notifications();
}

