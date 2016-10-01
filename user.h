#ifndef USER
#define USER

#pragma once

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QTimer>
#include <QTime>

class User
{
public:
    User(uint id,
         QHostAddress address,
         uint port,
         QString name);
    ~User();

    void connect();
    void disconnect();
    void setTime(QTime time);

    bool isConnected() const;
    QString getName() const;
    QHostAddress getIP() const;
    QTime getTime() const;
    uint getPort() const;
    uint getId() const;

private:
    uint userId;
    QHostAddress userAddress;
    uint userPort;
    QString userName;
    QString message;
    QTime timeOfLastMsg;
    bool connected;
};

#endif // USER

