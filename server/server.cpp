#include <QTcpServer>
#include <QTcpSocket>
#include "server.h"

using namespace std;

Server::Server() {}
Server::~Server() {}

TcpServer::TcpServer(const string& host, int port) :
    m_host(host), m_port(port)
{
    m_server = new QTcpServer(this);
}

bool TcpServer::start()
{
    return m_server->listen(QHostAddress(QString::fromStdString(m_host)),
                            static_cast<quint16>(m_port));
}

bool TcpServer::stop()
{
    m_server->close();
    return true;
}

void TcpServer::waitUntilReady()
{
   while (m_clients.size() < 2)
   {
       m_server->waitForNewConnection(-1);
       QTcpSocket* socket = m_server->nextPendingConnection();
       socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
       m_clients << socket;
   }
}

bool TcpServer::read(int client, string& data, int timeout)
{
    if (client < 0 || client >= m_clients.size())
        return false;

    QTcpSocket* socket = m_clients[client];
    if (socket->state() != QTcpSocket::ConnectedState)
        return false;

    while (socket->bytesAvailable() < 4)
    {
        bool ret = socket->waitForReadyRead(timeout);
        if (!ret)
            throw ServerTimeoutException("Le client a mis trop de temps à répondre");
        if (socket->state() != QTcpSocket::ConnectedState)
            return false;
    }
    QByteArray tmp = socket->read(4);
    qint32 size;
    QDataStream ds(&tmp, QIODevice::ReadWrite);
    ds >> size;

    while (socket->bytesAvailable() < size)
    {
        bool ret = socket->waitForReadyRead(timeout);
        if (!ret)
            throw ServerTimeoutException("Le client a mis trop de temps à répondre");
        if (socket->state() != QTcpSocket::ConnectedState)
            return false;
    }
    tmp = socket->read(size);
    data = string(tmp.data());

    return true;
}

bool TcpServer::write(int client, const string& data, int timeout)
{
    if (client < 0 || client >= m_clients.size())
        return false;

    QTcpSocket* socket = m_clients[client];
    if (socket->state() != QTcpSocket::ConnectedState)
        return false;

    QByteArray array;
    array.append(data.c_str());

    QByteArray temp;
    QDataStream ds(&temp, QIODevice::ReadWrite);
    ds << array.size();
    int ret = socket->write(temp);
    if (ret < 0) return false;

    ret = socket->write(array);
    if (ret < 0) return false;

    ret = socket->waitForBytesWritten(timeout);
    if (!ret)
        throw ServerTimeoutException("Le client a mis trop de temps à répondre");

    return ret;
}
