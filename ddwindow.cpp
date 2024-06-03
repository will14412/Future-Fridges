#include "ddwindow.h"
#include "chefswindow.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_ddwindow.h"
#include "database.h"

#include <qsqldatabase.h>
#include <QMessageBox>

DDWindow::DDWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DDWindow)
{
    ui->setupUi(this);

    refreshFoodStockList();
}

DDWindow::~DDWindow()
{
    delete ui;
}

void DDWindow::refreshFoodStockList()
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

void DDWindow::on_FoodStockList_itemClicked(QListWidgetItem *item)
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

bool DDWindow::validateSelectedItem()
{
    if (selectedName.isEmpty() || selectedCount == 0)
    {
        QMessageBox::warning(this, "Action", "No item selected.");
        qDebug() << "No item selected";
        return false;
    }
    return true;
}

void DDWindow::on_ChefSwitchButton_clicked()
{
    ChefsWindow *chefsWindow = new ChefsWindow();
    this->hide();
    chefsWindow->show();
}

void DDWindow::on_AddButton_clicked()
{
    /* Same as mentioned in the chefswindow function of the same name */

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
