#include "server.h"
#include "ui_server.h"


Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    this->setWindowTitle("Server");
    myServer = new QTcpServer(this);
    connect(ui->start,SIGNAL(clicked()),this,SLOT(serverStart()));
    connect(ui->stop,SIGNAL(clicked()),this,SLOT(stopServer()));
    ui->stop->setEnabled(false);
    connect(myServer,SIGNAL(newConnection()),this,SLOT(newConnectionUser()));



    length=0;
    command = -1;
    width = -1;
    height = -1;
    flag = false;
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
        qDebug() << address.toString();
        adr=address.toString();
    }

}

Server::~Server()
{
    stopServer();
    delete myServer;
    delete ui;
}

void Server::serverStart()
{
    qDebug()<<"Server start";
    if(myServer->listen(QHostAddress("192.168.1.199"),9093))
    {
        qDebug()<<"Server start";
        QString text=QTime::currentTime().toString()+">>"+"server started in "+ myServer->serverAddress().toString()+"; port: "+ QString::number(myServer->serverPort());
        ui->serverLog->addItem(text);
        ui->start->setEnabled(false);
        ui->stop->setEnabled(true);
    }
}

void Server::stopServer()
{

    myServer->close();
    camera->deleteLater();
    QString text=QTime::currentTime().toString()+">>"+"server stoped";
    ui->serverLog->addItem(text);
    ui->start->setEnabled(true);
    ui->stop->setEnabled(false);
}

void Server::newConnectionUser()
{
    camera = myServer->nextPendingConnection();
    connect(camera,SIGNAL(readyRead()),this,SLOT(readMsg()));
    connect(camera,SIGNAL(disconnected()),this,SLOT(disconnectUser()));
}

void Server::disconnectUser()
{
    if (camera)
    {
        camera->deleteLater();
    }
}

void Server::readMsg()
{
    QDataStream in(camera);
    //ready full comand
    if(!read(command)) return;

    qDebug()<<(int)command;

    switch(command)
    {
        case HELLO_SERVER:
        {
            if(!flag)
            {
                qDebug()<<"Client connect";
                sendMsg(HELLO_CLIENT);
                qDebug()<<"Client connect";
                flag = true;
            }
            qDebug()<<(int)sizeof(int);

            if(camera->bytesAvailable()<(int)sizeof(int))
            {

                qDebug()<<camera->bytesAvailable();
                return;
            }
            in>>length;
            qDebug()<<length;
            break;
        }
        case GB_SERVER:
        {
            sendMsg(GB_CLIENT);
            break;
        }
        case LENGTH_CHANGE:
        {
            if(!read(length)) return;
            sendMsg(COMAND_EXECUTED);
            break;
        }
        case FONE_RESET:
        {
            //reset fone
            sendMsg(COMAND_EXECUTED);
            break;
        }
        case ERROR:
        {
            //catch
            stopServer();
            break;
        }
        case DATA:
        {
            //play video
            int currentSize = 0;

            if(!read(width)) return;
            if(!read(height)) return ;
            qDebug()<<"width:"<<width<<" height "<<height;

            while(currentSize < length)
            {
                int tmp;
                if(!read(tmp)) continue;
                data.push_back(tmp);
            }

            sendMsg(DATA_RECEIVED);
            break;
        }
   }
}

bool Server::read(char &var)
{
    QDataStream in(camera);
    if(var == -1)
    {
        if (camera->bytesAvailable() < (int)sizeof(char))
            return false;
         qDebug()<<camera->bytesAvailable();
        //qint8 tmp;
        char* tmp;
        qDebug()<<"lolz";
        in.device()->reset();
        in.readRawData(tmp,(int)sizeof(char));
        qDebug()<<(char)*tmp;
        //in>>tmp;
        var=(char)*tmp;
        qDebug() << "Received command ";
    }
    return true;
}

bool Server::read(int &var)
{
    QDataStream in(camera);
    if(var == -1)
    {
        if (camera->bytesAvailable() < (int)sizeof(int))
            return false;
        qDebug()<<camera->bytesAvailable();
        //in>>var;
        char* tmp;
        qDebug()<<"lolz";
        in.device()->reset();
        in.readRawData(tmp,(int)sizeof(int));
        qDebug()<<(int)*tmp;
        //in>>tmp;
        var=(int)*tmp;
        qDebug() << "Received command ";
    }
    return true;
}

void Server::sendMsg(COMMAND cmd)
{
    QByteArray msg;
    QDataStream out(&msg, QIODevice::WriteOnly);
    out<<(char)cmd;
    camera->write(msg);
}
