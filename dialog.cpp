#include "dialog.h"
#include "ui_dialog.h"

#include <QTime>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QUdpSocket udpSocket = new QUdpSocket(this);
    udpSocket->bind(Ui::port, Ui::address);
    connect(udpSocket, SIGNAL(readyRead()),this, SLOT(startReading()));

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::checkUser(int num)
{
    // Check num
    if(num >= usersList.size())
        return;

    // If user is not connected,
    // it's unnecessary to disconnect him
    if(!usersList.at(num).isConnected())
        return;

    // If his time has not yet expired,
    // let's give him a chance
    if(usersList.at(num).getTime().addSecs(Ui::waitUser) <
            QTime::currentTime())
    {
        return;
    }

    // TODO: some manipulations
    usersList[num].disconnect();
}

void Dialog::createUser(QHostAddress address, QString name)
{
    User *newUser = new User(address, name);
    usersList.append(newUser);
    usersList.[usersList.size()-1].connect();
}

void Dialog::startReading()
{
while (udpSocket->hasPendingDatagrams())
{
    QByteArray datagram;
    datagram.resize(udpSocket->pendingDatagramSize());
    udpSocket->readDatagram(datagram.data(), datagram.size());
    if(datagram.startsWith('?'))
    {
        uint id;
        QHostAddress address;
        uint port;
        QString name;
        switch(datagram.at(1))
        {
            case '1': // id
                datagram.remove(0, 2);
                id = datagram.toUInt();
                break;
            case '2': // Host address
                datagram.remove(0, 2);
                // Надо проверить
                address = QHostAddress(QString(datagram.data()));
                break;
            case '3':
                datagram.remove(0, 2);
                port = datagram.toUInt();
                break;
            case '4':
                datagram.remove(0, 2);
                port = QString(datagram.data());
                break;
        }
        createUser(id, address, port, name);
    }
// Отослать всем
    }
   }
