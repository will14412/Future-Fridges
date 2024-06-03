#ifndef CHEFSWINDOW_H
#define CHEFSWINDOW_H

#include <QMainWindow>
#include "qlistwidget.h"

namespace Ui {
class ChefsWindow;
}

class ChefsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChefsWindow(QWidget *parent = nullptr);
    ~ChefsWindow();

private slots:
    void on_DDSwitchButton_clicked();
    void on_AddItemButton_clicked();
    void on_RemoveItemButton_clicked();
    void on_AdminActionsButton_clicked();
    void on_AddButton_clicked();
    void on_RemoveButton_clicked();
    void refreshFoodStockList();

    void on_FoodStockList_itemClicked(QListWidgetItem *item);

private:

    QString selectedName; // Member variable to store selected item's name
    int selectedCount;    // Member variable to store selected item's count


    Ui::ChefsWindow *ui;
    bool validateSelectedItem();
};

#endif // CHEFSWINDOW_H
