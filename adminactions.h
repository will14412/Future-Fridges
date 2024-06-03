#ifndef ADMINACTIONS_H
#define ADMINACTIONS_H

#include <QMainWindow>
#include "qlistwidget.h"


namespace Ui {
class adminactions;
}

class adminactions : public QMainWindow
{
    Q_OBJECT

public:
    explicit adminactions(QWidget *parent = nullptr);
    ~adminactions();

private slots:
    void on_ReorderButton_clicked();

    void on_HealthSafetyButton_clicked();

    void on_LogoutButton_clicked();


    void on_FoodStockList_itemClicked(QListWidgetItem *item);

    void refreshFoodStockList();



    void on_UserManagementButton_clicked();

    void on_SubmitButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::adminactions *ui;

    QString selectedName; // Member variable to store selected item's name
    int selectedCount;    // Member variable to store selected item's count

    void notifications();
    bool validateSelectedItem();
};

#endif // ADMINACTIONS_H
