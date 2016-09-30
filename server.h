#ifndef SERVER_H
#define SERVER_H

#pragma once

#include <QList>
#include <QTimer>
#include <QLabel>
#include <QWidget>
#include <QUdpSocket>
#include <QListWidget>
#include <QPushButton>
#include <QHostAddress>

#include "user.h"

namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

    void createUser(QHostAddress address, uint port,
                    uint id, QString name);

signals:
    void userListChanged();

private slots:
    void start();
    void showUsers();
    void pingUsers();
    void checkUsers();
    void initServer();
    void closeServer();
    void startReading();
    bool parseMessage(QByteArray message,
                      QHostAddress address);

private:
    Ui::Server *ui;

    QLabel *infoL;
    QLabel *pathL;
    QLabel *usersL;
    QPushButton *quitBut;
    QPushButton *startBut;
    QListWidget *listOfUsersLW;

    QTimer *pingTimer;
    QTimer *silenceTimer;
    QUdpSocket *inSocket;
    QUdpSocket *outSocket;
    QList<User> usersList;
};

#endif // SERVER_H
