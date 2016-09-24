#ifndef DIALOG_H
#define DIALOG_H

#pragma once

#include <QList>
#include <QTimer>
#include <QLabel>
#include <QDialog>
#include <QUdpSocket>
#include <QHostAddress>

#include "user.h"

namespace Ui
{
    class Dialog;
}

class Server : public QDialog
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

    void createUser(QHostAddress address, uint port,
                    uint id, QString name);
    void checkUser(int num);

private slots:
    void initServer();
    void startReading();
    bool parseMessage(QByteArray message,
                      QHostAddress address,
                      quint16 port);

private:
    Ui::Dialog *ui;

    QLabel *info;

    QTimer *timer;
    QUdpSocket *inSocket;
    QUdpSocket *outSocket;
    QList<User> usersList;
};

#endif // DIALOG_H
