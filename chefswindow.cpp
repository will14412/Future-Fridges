#include "chefswindow.h"
#include "mainwindow.h"
#include "otpwindow.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_chefswindow.h"
#include "additemdialog.h"
#include "database.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QString>
#include <string>
#include <vector>
#include <qsqldatabase.h>

ChefsWindow::ChefsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChefsWindow)
{
    ui->setupUi(this);

    refreshFoodStockList();
}

ChefsWindow::~ChefsWindow()
{
    delete ui;
}

/* When an item is selected, it should appear in the SelectedLabel.
Any adjustments in the Edit Box should impact only the selected item. */

// Switch to Delivery Driver button navigates to the OTP Window (otpwindow.ui)
void ChefsWindow::on_DDSwitchButton_clicked()
{
    OTPWindow *otpWindow = new OTPWindow();
    this->hide();
    otpWindow->show();
}

void ChefsWindow::on_AddItemButton_clicked()
{
    /* This button adds a new item into the database. Not to be confused
    with the add amount button (which just increases stock). This button
    should open a new window form for all the details. */

    AddItemDialog addItemDialog;
    addItemDialog.exec();

    // check add item dialog is closed
    refreshFoodStockList();
}

void ChefsWindow::refreshFoodStockList()
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

    ui->FoodStockList->clearSelection();
    ui->SelectedLabel->setText("Currently Selected: ");
    selectedName = "";
    selectedCount = 0;
}

void ChefsWindow::on_FoodStockList_itemClicked(QListWidgetItem *item)
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

bool ChefsWindow::validateSelectedItem()
{
    if (selectedName.isEmpty())
    {
        QMessageBox::warning(this, "Action", "No item selected.");
        qDebug() << "No item selected";
        return false;
    }
    return true;
}

void ChefsWindow::on_RemoveItemButton_clicked()
{
    if (!validateSelectedItem())
    {
        return;
    } else
    {
        if(deleteStock(selectedName,selectedCount))
        {
            QMessageBox::information(this,"Success","Successfully deleted item: " + selectedName);
            selectedName.clear();
            selectedCount = 0;

        } else
        {
        QMessageBox::warning(this, "Deletion Error", "Failed to remove the item.");
        }
    }

    refreshFoodStockList();
}

// Admin Actions button navigates to the Login Window (mainwindow.ui)
void ChefsWindow::on_AdminActionsButton_clicked()
{
    MainWindow *mainWindow = new MainWindow();
    this->hide();
    mainWindow->show();
}

void ChefsWindow::on_AddButton_clicked()
{
    /* This button is what adds to the stock of existing items within the
    database. The amount is withdrawn from the numerical line edit and the
    exact item is selected in the list above this button. */

    int amount = ui->AddSpinBox->value();

    if (!validateSelectedItem())
    {
        return;
    } else
    {
        updateStock(selectedName, amount);
    }

    refreshFoodStockList();
}

void ChefsWindow::on_RemoveButton_clicked()
{
    /* Same as the add button, except the reverse. These two functions
    should be very similar. */

    int amount = ui->RemoveSpinBox->value();

    if (amount > selectedCount)
    {
        amount = selectedCount;
    }

    if (!validateSelectedItem())
    {
        return;
    } else
    {
        updateStock(selectedName, -amount);
    }

    refreshFoodStockList();
}



