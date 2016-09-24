#ifndef DIALOG_H
#define DIALOG_H

#pragma once

#include <QDialog>
#include <QTimer>
#include <QHostAddress>
#include <QList>
#include <QUdpSocket>
#include "user.h"

namespace Ui {

// Time for user's reaction (secs)
int waitUser = 180;
// Server's port
int port = 45454;
QHostAddress addrServer = QHostAddress("192.168.0.1");

class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    void createUser(QHostAddress address, QString name);
    void checkUser(int num);

private:
    Ui::Dialog *ui;

    QTimer *timer;
    QList<User> usersList;
};

#endif // DIALOG_H
