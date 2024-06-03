#ifndef DDWINDOW_H
#define DDWINDOW_H

#include "qlistwidget.h"
#include <QMainWindow>

namespace Ui {
class DDWindow;
}

class DDWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DDWindow(QWidget *parent = nullptr);
    ~DDWindow();

private slots:
    void on_ChefSwitchButton_clicked();
    void refreshFoodStockList();
    void on_FoodStockList_itemClicked(QListWidgetItem *item);
    void on_AddButton_clicked();

private:
    QString selectedName; // Member variable to store selected item's name
    int selectedCount;    // Member variable to store selected item's count

    bool validateSelectedItem();

    Ui::DDWindow *ui;
};

#endif // DDWINDOW_H
