#include "chefswindow.h"
#include "database.h"
#include "qsqlerror.h"


#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDate>
#include <QRandomGenerator>
#include <QCryptographicHash>

struct StockInfo
{
    QString name;
    int count;
    QString date;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); // Use the appropriate database driver (e.g., QMYSQL for MySQL)
    db.setDatabaseName("mydatabase.db"); // Set the name of your database file

    if (!db.open())
    {
        qDebug() << "Error: Unable to open the database";
        return a.exec();
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS stock (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, "
               "count INTEGER, metric TEXT, date DATE, active INTEGER, inactive INTEGER)");

    ChefsWindow w;
    w.show();
    return a.exec();
}

bool addStock(QString name, int count, QString metric, QDate date)
{
    // Check if name exists in stock
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT name FROM stock WHERE name = (:name)");
    checkQuery.bindValue(":name", name);
    if (checkQuery.exec() && checkQuery.next())
    {
        qDebug() << "Stock already exists";
        return false; // Indicate stock already exists
    }

    // Seed the random number generator
    QRandomGenerator randomGenerator(static_cast<quint64>(QTime::currentTime().msec()));

    int randomValue = randomGenerator.bounded(10001);

    QSqlQuery query;
    //query.exec("CREATE TABLE IF NOT EXISTS stock (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, "
    //           "count INTEGER, metric TEXT, date DATE, activeOTP INT, inactiveOTP INT)");

    query.prepare("INSERT INTO stock (name, count, metric, date, inactive) VALUES (:name, :count, :metric, :date, :inactive)");
    query.bindValue(":name", name);
    query.bindValue(":count", count);
    query.bindValue(":metric", metric);
    query.bindValue(":date", date);
    query.bindValue(":inactive", randomValue);


    if (!query.exec())
    {
        qDebug() << "Error executing insert query:" << query.lastError().text();
        return false; // Indicate failure
    }

    return true; // Indicate success
}

bool deleteStock(QString name, int count)
{
    QSqlQuery query;
    query.prepare("DELETE FROM stock WHERE name = :name AND count = :count");

    query.bindValue(":name", name);
    query.bindValue(":count", count);

    if (!query.exec())
    {
        qDebug() << "Error executing DELETE query:" << query.lastError().text();

        return false; // Return false indicating deletion failure
    }

    qDebug() << "Deletion successful for" << name << "with count" << count;

    return true;
}

void updateStock(QString name, int count)
{
    //If removing stock then count needs to be negative

    QSqlQuery query;

    query.prepare("SELECT count FROM stock WHERE name = :name");
    query.bindValue(":name", name);
    query.exec();

    int countFind = 0;
    while (query.next())
    {
        countFind = query.value(0).toInt();
    }

    query.prepare("UPDATE stock SET count = :count WHERE name = :name");
    query.bindValue(":name", name);
    query.bindValue(":count", (countFind + count));

    query.exec();
}

StockInfo selectStock()
{

    QSqlQuery query;

    StockInfo stockInfo; // Create a structure to hold the information

    query.exec("SELECT name, count, date FROM stock");
    while (query.next())
    {
        stockInfo.name = query.value(0).toString(); // Use index 0 for the first column
        stockInfo.count = query.value(1).toInt();   // Use index 1 for the second column
        stockInfo.date = query.value(2).toString(); // Use index 2 for the third column
    }

    return stockInfo;
}

bool login(QString username, QString password)
{

    QByteArray passwordBytes = password.toUtf8();
    QByteArray hashedPassword = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
    QSqlQuery query;

    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", hashedPassword);
    query.exec();

    while (query.next())
    {
        return true;
    }

    return false;
}

bool signup (QString username, QString password) {

    QSqlQuery query;

    qDebug() << "Username: " << username;
    qDebug() << "Password: " << password;
    query.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username "
               "TEXT, password TEXT)"); // Fix the column type

    query.prepare("SELECT * FROM users WHERE username = :username");
    query.bindValue(":username", username);
    query.exec();

    // Check if the username already exists
    query.prepare("SELECT * FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (query.exec() && query.next())
    {
        qDebug() << "Username already exists.";
        return false; // Username exists, return false
    }

    QByteArray passwordBytes = password.toUtf8();
    QByteArray hashedPassword = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);

    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", hashedPassword);

    query.exec();

    return true;
}

bool insertDriver(QString email)
{
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS driver (id INTEGER PRIMARY KEY AUTOINCREMENT, email TEXT, date DATE)");

    query.prepare("INSERT INTO driver (email, date) VALUES (:email, NULL)");
    query.bindValue(":email", email);


    if (query.exec())
    {
        qDebug() << " Successfully added driver";
        return true;
    } else
    {
        qDebug() << " failed driver add";
        qDebug() << query.lastError();
        return false;

    }

}

bool removeDriver(QString email)
{
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS driver (id INTEGER PRIMARY KEY AUTOINCREMENT, email TEXT, date DATE)");

    query.prepare("DELETE FROM driver WHERE email = :email");
    query.bindValue(":email", email);

    return query.exec();
}

bool selectDriver(QString email)
{
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS driver (id INTEGER PRIMARY KEY AUTOINCREMENT, email TEXT, date DATE)");

    query.prepare("SELECT email FROM driver WHERE email = :email");
    query.bindValue(":email", email);

    if (query.exec() && query.next())
    {
        qDebug() << "Already a user";
        qDebug() << query.lastError();

        return true;
    }

    qDebug() << query.lastError();
    return false;
}

QString checkExpired()
{
    QSqlQuery query;
    QString result;

    // Get the current date
    QDate currentDate = QDate::currentDate();

    // Execute the query to retrieve records where the date is past the current date
    query.prepare("SELECT name, date FROM stock WHERE date <= :currentDate");
    query.bindValue(":currentDate", currentDate);

    if (query.exec())
    {
        while (query.next())
        {
            QString name = query.value(0).toString();
            QString date = query.value(1).toString();
            result += "\n" + name + " expired on " + date;
        }
    } else
    {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

    // Return the formatted results
    return result;
}

QString checkExpiring()
{
    QSqlQuery query;
    QString result;

    // Get the current date
    QDate tomorrowsDate = QDate::currentDate().addDays(1);

    // Execute the query to retrieve records where the date is past the current date
    query.prepare("SELECT name FROM stock WHERE date = :tomorrowsDate");
    query.bindValue(":tomorrowsDate", tomorrowsDate);

    if (query.exec())
    {
        while (query.next())
        {
            QString name = query.value(0).toString();
            result += "\n" + name + " expires tomorrow";
        }
    } else
    {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

    // Return the formatted results
    return result;
}

QString checkStock()
{
    QSqlQuery query;
    QString result;

    // Execute the query to retrieve records where the date is past the current date
    query.prepare("SELECT name FROM stock WHERE count < 5");

    if (query.exec())
    {
        while (query.next())
        {
            QString name = query.value(0).toString();
            result += "\n" + name + " is running low";
        }
    } else
    {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

    // Return the formatted results
    return result;
}
