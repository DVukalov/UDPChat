#include <QDebug>
#include "user.h"

User::User(uint id,
           QHostAddress address,
           uint port,
           QString name) :
    userId(id),
    userAddress(address),
    userPort(port),
    userName(name)
{
    __print  << this->userId
             << this->userAddress.toString()
             << this->userPort
             << this->userName;
}

User::~User()
{
    __print;
}

void User::connect()
{
    __print;
    connected = true;
    timeOfLastMsg = QTime::currentTime();
}

void User::disconnect()
{
    __print;
    connected = false;
}

bool User::isConnected() const
{
    return connected;
}

QString User::getName() const
{
    return userName;
}

QHostAddress User::getIP() const
{
    return userAddress;
}

QTime User::getTime() const
{
    return timeOfLastMsg;
}

uint User::getPort() const
{
    return userPort;
}

uint User::getId() const
{
    return userId;
}

void User::setTime(QTime time)
{
    timeOfLastMsg = time;
}
