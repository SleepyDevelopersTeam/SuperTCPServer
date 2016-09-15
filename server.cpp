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



    length=-1;
    command = -1;
    width = -1;
    height = -1;
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            //qDebug() << address.toString();
            adr=address.toString();
        if(adr[0]=='1' && adr[1]=='9' && adr[2]=='2')
            break;
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
    if(myServer->listen(QHostAddress(adr),9090))
    {
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
    if(!read(command)) return;

    switch(command)
    {
        case HELLO_SERVER:
        {
            sendMsg(HELLO_CLIENT);
            command=-1;
            break;
        }
        case GB_SERVER:
        {
            sendMsg(GB_CLIENT);
            command=-1;
            stopServer();
            break;
        }
        case LENGTH_CHANGE:
        {
            if(!read(length)) return;
           // qDebug()<<"length "<<length;
            sendMsg(COMAND_EXECUTED);
            command=-1;
            break;
        }
        case FONE_RESET:
        {
            //reset fone
            sendMsg(COMAND_EXECUTED);
            command=-1;
            break;
        }
        case ERROR:
        {
            //catch
            stopServer();
            command=-1;
            break;
        }
        case DATA:
        {
            data.clear();
            //play video
            int currentSize = 0;

            if(!read(width)) return;
            if(!read(height)) return ;
            if(camera->bytesAvailable()<length) return;
            QByteArray arr=camera->readAll();
            for(int i=0;i<arr.length();++i)
            {
                data.push_back(arr[i]);
            }
            sendMsg(DATA_RECEIVED);
            imageObtained();
            command=-1;
            width=-1;
            height=-1;
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
        qint8 tmp;
        in>>tmp;
       // qDebug()<<"command "<<tmp;
        var=(char)tmp;
    }
    return true;
}

bool Server::read(short &var)
{
    QDataStream in(camera);
    if(var == -1)
    {
        if (camera->bytesAvailable() < (int)sizeof(short))
            return false;
        qint16 tmp;
        in>>tmp;
        //qDebug()<<"command "<<tmp;
        var=(short)tmp;
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
        in>>var;
      //  qDebug()<<"data: "<<var;
    }
    return true;
}

void Server::sendMsg(COMMAND cmd)
{
    QByteArray msg;
    QDataStream out(&msg, QIODevice::WriteOnly);
    out<<(quint8)cmd;
    camera->write(msg);
}
