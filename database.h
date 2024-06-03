#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QDate>

bool login(QString username, QString password);
bool signup(QString username, QString password);
bool addStock(QString name, int count, QString metric, QDate date);
bool deleteStock(QString name, int count);
void updateStock(QString name, int count);
bool insertDriver(QString email);
bool removeDriver(QString email);
bool selectDriver(QString email);

// Checks for Notifications
QString checkExpired();
QString checkExpiring();
QString checkStock();

#endif // DATABASE_H
