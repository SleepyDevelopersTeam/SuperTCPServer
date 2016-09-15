#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTime>
#include <QTcpSocket>
#include <vector>
#include <QVector>
#include <QMap>
#include <QThread>
#include <QDataStream>
#include <QDebug>
#include <QNetworkInterface>
#include <QAbstractSocket>



namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();
    QTcpServer* myServer;
    short width;
    short height;
    QVector<char> data;


private:
    Ui::Server *ui;
    QTcpSocket* camera;
    enum COMMAND
    {
        DATA = 0x00,

        HELLO_SERVER = 0x1E,
        HELLO_CLIENT = 0x1A,

        DATA_RECEIVED = 0x2D,

        LENGTH_CHANGE = 0x31,
        FONE_RESET = 0x3F,
        COMAND_EXECUTED = 0x3E,

        GB_SERVER = 0x45,
        GB_CLIENT = 0x4C,

        ERROR = 0x66
    };

    void sendMsg(QString message);

    bool read(int &var);
    bool read(char &var);
    bool read(short &var);

    int length;

    char command;

    QString adr;
private slots:
    void serverStart();
    void stopServer();
    void disconnectUser();
    void readMsg();
    void sendMsg(COMMAND cmd);
    void newConnectionUser();
signals:
    void imageObtained();

};

#endif // SERVER_H
