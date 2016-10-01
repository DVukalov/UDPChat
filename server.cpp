#include "server.h"
#include "ui_server.h"

#include <QTime>
#include <QList>
#include <QDebug>
#include <QLayout>
#include <QSizePolicy>

namespace
{
    // Time for user's reaction (secs)
    const uint waitUser = 20;
    // Time for ping user (secs)
    const uint pingTime = 30;
    // Server's port
    uint serverPort = 45454;
    // Server's address
    QHostAddress serverAddress = QHostAddress("192.168.0.1");
    // ID for users
    uint userId = 0;
    // Regular expressions for messages
    const QByteArray serverInit = "INITSERVER";
    const QByteArray userInit = "INITUSER";
    const QByteArray errorMsg = "ERROR";
    const QByteArray msgToUser = "MSG";
    const QByteArray pingUser = "PING";
    const QByteArray separator = "#@#";

    enum Errors{
        eName   = 1,
        eFault  = 2
    };
}

Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    __print;
    // network elements
    ui->setupUi(this);
    inSocket = new QUdpSocket(this);
    outSocket = new QUdpSocket(this);

    // buttons
    quitBut = new QPushButton(tr("Quit"), this);
    startBut = new QPushButton(tr("Start"), this);
    startBut->setToolTip("Switch on server");
    quitBut->setToolTip("Quit from application");
    startBut->setToolTipDuration(3000);
    quitBut->setToolTipDuration(3000);
    connect(quitBut, SIGNAL(clicked()), this, SLOT(closeServer()));
    connect(startBut, SIGNAL(clicked()), this, SLOT(start()));

    // other elements
    listOfUsersLW = new QListWidget(this);
    connect(this, SIGNAL(userListChanged()), this, SLOT(showUsers()));
    pathL = new QLabel("Server host: 255.255.255.255\nServer port: 00000", this);
    usersL = new QLabel("List of users", this);
    infoL = new QLabel("Some info", this);
    infoL->setFont(QFont("Courier", 12));
    pathL->setFont(QFont("Courier", 12));
    usersL->setFont(QFont("Courier", 12));
    pathL->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    infoL->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    usersL->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    pathL->setFixedSize(300, 40);
    usersL->setFixedHeight(20);

    // checkers
    silenceTimer = new QTimer(this);
    silenceTimer->setInterval(::waitUser * 100);
    connect(silenceTimer, SIGNAL(timeout()), this, SLOT(checkUsers()));
    pingTimer = new QTimer(this);
    pingTimer->setInterval(::pingTime * 1000);
    connect(pingTimer, SIGNAL(timeout()), this, SLOT(pingUsers()));

    // layouts
    QVBoxLayout *labLayout = new QVBoxLayout;
    labLayout->addWidget(pathL);
    labLayout->addWidget(infoL);
    labLayout->addWidget(startBut);
    labLayout->addWidget(quitBut);

    QVBoxLayout *userLayout = new QVBoxLayout;
    userLayout->addWidget(usersL);
    userLayout->addWidget(listOfUsersLW);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(labLayout);
    mainLayout->addLayout(userLayout);
    this->setLayout(mainLayout);

    setWindowTitle(tr("UDP server"));

    __print << "start!";
}

Server::~Server()
{
    delete ui;
    delete infoL;
    delete pathL;
    delete usersL;
    delete quitBut;
    delete startBut;
    delete inSocket;
    delete outSocket;
    delete silenceTimer;
    delete listOfUsersLW;
}

void Server::start()
{
    inSocket->bind(::serverPort, QUdpSocket::ShareAddress);

    connect(inSocket, SIGNAL(readyRead()),
            this, SLOT(startReading()));

    initServer();
    startBut->setDisabled(true);

    silenceTimer->start();
    pingTimer->start();
}

void Server::initServer()
{
    outSocket->writeDatagram(::serverInit.data(), ::serverInit.size(),
                             QHostAddress::Broadcast, ::serverPort);
}

void Server::checkUsers()
{
    __print;
    for(int num = 0; num < usersList.size(); num++)
    {
        // If user is not connected,
        // it's unnecessary to disconnect him
        if(!usersList.at(num).isConnected())
            return;

        // If his time has not yet expired,
        // let's give him a chance
        if(usersList.at(num).getTime().addSecs(::waitUser) >
                QTime::currentTime())
        {
            return;
        }

        infoL->setText(infoL->text() +
                       QString("\nUser named %1 disconnected")
                       .arg(usersList.at(num).getName()));

        usersList[num].disconnect();

        emit userListChanged();
    }
}

void Server::createUser(QHostAddress address,
                        uint port, QString name)
{
    __print;

    for(int i = 0; i < usersList.size(); i++)
    {
        if(name == usersList.at(i).getName())
        {
            QByteArray errorDatagram = ::errorMsg + ::separator
                    + QByteArray::number(eName);

            outSocket->writeDatagram(errorDatagram.data(),
                                     errorDatagram.size(),
                                     usersList.at(i).getIP(),
                                     usersList.at(i).getPort());
            return;
        }
    }

    User *newUser = new User(userId, address, port, name);

    usersList.append(static_cast<const User>(*newUser));
    usersList[usersList.size()-1].connect();

    QByteArray initDatagram = ::userInit;

    outSocket->writeDatagram(initDatagram.data(),
                             initDatagram.size(),
                             usersList.at(userId).getIP(),
                             usersList.at(userId).getPort());

    infoL->setText(infoL->text() +
                   QString("\nWelcome user %1 !")
                   .arg(usersList.at(userId).getName()));
    userId++;
    delete newUser;
    newUser = NULL;
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

        __print << parseMessage(datagram, address);
    }
}

// Парсер сообщений
bool Server::parseMessage(QByteArray message,
                          QHostAddress address)
{
    // init server
    if(message.startsWith(::serverInit))
    {
        ::serverAddress = QHostAddress(address.toString().remove(0,7));

        __print << ::serverAddress.toString()
                << ::serverPort;

        pathL->setText(tr("Server host: %1").
                      arg(::serverAddress.toString())
                      + tr("\nServer port: %1").
                      arg(QString::number(::serverPort)));
        return true;
    }

    // init user
    // Format:
    // QByteArray("INITUSER#@#NAME#@#PORT")
    if(message.startsWith(::userInit))
    {
        QByteArray buf;
        QString name;
        uint port;
        int index;

        if(message.count(::separator) != 2)
        {
            __print << "Wrong user initialisation!";
            return false;
        }

        message.remove(0, ::userInit.size() + ::separator.size());

        index = message.indexOf(::separator);

        // Get user's name
        qCopy(message.begin(),
              message.begin() + index,
              buf.begin());

        if(buf.data() == NULL)
        {
            __print << "Empty name!";
            return false;
        }

        name = QString(buf.data());

        message.remove(0, index + ::separator.size());

        qCopy(message.begin(), message.end(), buf.begin());

        port = atoi(buf.data());

        createUser(QHostAddress(address.toString().remove(0,7)),
                   port, name);

        emit userListChanged();

        return true;
    }

    // send to all
    // Format:
    // QByteArray("MSG#@#NAME#@#TEXT")
    if(message.startsWith(::msgToUser))
    {
        if(message.count(::separator) != 2)
        {
            __print << "Message corrupted!";
            return false;
        }

        QByteArray buf;
        QString name;
        int indexB;
        int indexE;

        indexB = message.indexOf(::separator);
        indexE = message.lastIndexOf(::separator);
        qCopy(message.begin() + indexB + ::separator.size(),
              message.begin() + indexE, buf.begin());
        name = QString(buf.data());

        for(int i = 0; i < usersList.size(); i++)
        {
            if(usersList.at(i).getName() == name)
            {
                if(!usersList.at(i).isConnected())
                {
                    usersList[i].connect();
                    emit userListChanged();
                }
            }
        }

        for(int i = 0; i < usersList.size(); i++)
        {
            if(name == usersList.at(i).getName())
            {
                usersList[i].setTime(QTime::currentTime());
            }

            outSocket->writeDatagram(message.data(), message.size(),
                                     usersList.at(i).getIP(),
                                     usersList.at(i).getPort());
        }

        return true;
    }

    return false;
}

void Server::showUsers()
{
    listOfUsersLW->clear();
    for(int i = 0; i < usersList.size(); i++)
    {
        listOfUsersLW->addItem((usersList.at(i).isConnected()
                                ? "(Here) " : "(Afk)  ")
                               + usersList.at(i).getName());
    }
}

void Server::pingUsers()
{
    __print;
    QByteArray pingMessage = ::pingUser;
    for(int i = 0; i < usersList.size(); i++)
    {
        if(usersList.at(i).isConnected())
        {
            outSocket->writeDatagram(pingMessage.data(),
                                     pingMessage.size(),
                                     usersList.at(i).getIP(),
                                     usersList.at(i).getPort());
        }
    }
}

void Server::closeServer()
{
    __print;
    QByteArray pingMessage = ::errorMsg + ::separator
            + QByteArray::number(eFault);
    for(int i = 0; i < usersList.size(); i++)
    {
        if(usersList.at(i).isConnected())
        {
            outSocket->writeDatagram(pingMessage.data(),
                                     pingMessage.size(),
                                     usersList.at(i).getIP(),
                                     usersList.at(i).getPort());
        }
    }
    this->close();
}
