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
    timer = NULL;
    __print  << this->userId
             << this->userAddress.toString()
             << this->userPort
             << this->userName;
}

User::~User()
{
    __print;
    delete timer;
}

void User::connect()
{
    __print;
    // TODO: some interface
    udpSocket = new QUdpSocket();

    connected = true;
}

void User::disconnect()
{
    __print;
    // TODO: some interface
    udpSocket->close();
    delete udpSocket;
    udpSocket = NULL;

    connected = false;
}

bool User::sendMessage(QByteArray message)
{
    __print << message.data();
    udpSocket->writeDatagram(message.data(), message.size(),
                             userAddress, userPort);
    return true;
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
