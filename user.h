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
    bool sendMessage(QByteArray message);

    bool isConnected() const;
    QString getName() const;
    QString getIP() const;
    QTime getTime() const;
    uint getPort() const;
    uint getId() const;

private:
    uint userId;
    QHostAddress userAddress;
    uint userPort;
    QString userName;
    QUdpSocket* udpSocket;
    QString message;
    QTimer *timer;
    QTime timeOfLastMsg;
    bool connected;
};

#endif // USER

