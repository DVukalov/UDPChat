#include "server.h"
#include "ui_dialog.h"

#include <QTime>
#include <QList>
#include <QDebug>

using namespace Ui;

namespace
{
    // Time for user's reaction (secs)
    uint waitUser = 180;
    // Server's port
    uint serverPort = 45454;
    // Server's address
    QHostAddress serverAddress = QHostAddress("192.168.0.1");
    // ID for users
    uint userId = 1;
    // Regular expressions for messages
    QByteArray serverInit = "INITSERVER";
    QByteArray userInit = "INITUSER";
    QByteArray msgToUser = "MSG";
    QByteArray pingUser = "PING";
    QByteArray separator = "#@#"; //???
}

Server::Server(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    inSocket = new QUdpSocket(this);
    outSocket = new QUdpSocket(this);
    info = new QLabel(this);
    // временно
    info->setGeometry(1, 1, 200, 30);

    inSocket->bind(::serverPort, QUdpSocket::ShareAddress);

    connect(inSocket, SIGNAL(readyRead()),
            this, SLOT(startReading()));

    initServer();

    __print << "start!";
}

Server::~Server()
{
    delete ui;
    delete inSocket;
    delete outSocket;
}

void Server::initServer()
{
    outSocket->writeDatagram(::serverInit.data(), ::serverInit.size(),
                             QHostAddress::Broadcast, ::serverPort);
}

void Server::checkUser(int num)
{
    __print;
    // Check num
    if(num >= usersList.size())
        return;

    // If user is not connected,
    // it's unnecessary to disconnect him
    if(!usersList.at(num).isConnected())
        return;

    // If his time has not yet expired,
    // let's give him a chance
    if(usersList.at(num).getTime().addSecs(::waitUser) <
            QTime::currentTime())
    {
        return;
    }

    // TODO: some manipulations
    usersList[num].disconnect();
}

void Server::createUser(QHostAddress address, uint port,
                        uint id, QString name)
{
    __print;
    // TODO: Проверка на одинаковых пользователей
    usersList.append(User(id, address, port, name));
    usersList[usersList.size()-1].connect();
}

void Server::startReading()
{
    while (inSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        quint16 port;
        QHostAddress address;

        datagram.resize(inSocket->pendingDatagramSize());
        inSocket->readDatagram(datagram.data(), datagram.size(),
                                &address, &port);

        __print << parseMessage(datagram, address, port);
    }
}

// Парсер сообщений
bool Server::parseMessage(QByteArray message,
                          QHostAddress address,
                          quint16 port)
{
    // init server
    if(message.startsWith(::serverInit))
    {
        ::serverAddress = QHostAddress(address.toString().remove(0,7));

        __print << ::serverAddress.toString()
                << ::serverPort;

        info->setText(tr("Server host: %1").
                      arg(::serverAddress.toString())
                      + tr("\nServer port: %1").
                      arg(QString::number(::serverPort)));
        return true;
    }

    // init user
    // Format:
    // QByteArray("INITUSER#@#NAME")
    if(message.startsWith(::userInit))
    {
        QByteArray buf;
        QString name;

        if(message.count(::separator) != 1)
        {
            __print << "Wrong user initialisation!";
            return false;
        }

        message.remove(0, ::userInit.size() + ::separator.size());

        // Get user's name
        qCopy(message.begin(), message.end(), buf.begin());
        name = QString(buf.data());

        createUser(QHostAddress(address.toString().remove(0,7)),
                   port, ::userId, name);

        ::userId++;

        return true;
    }

    // send to all
    if(message.startsWith(::msgToUser))
    {
        if(message.size() == ::msgToUser.size())
        {
            __print << "Empty message!";
            return false;
        }

        for(int i = 0; i < usersList.size(); i++)
        {
            usersList[i].sendMessage(message);
        }

        return true;
    }

    return false;
}
