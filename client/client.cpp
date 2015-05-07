#include <QHostAddress>
#include <QTcpSocket>
#include "client.h"

using namespace std;

Client::Client() { }
Client::~Client() { }

TcpClient::TcpClient(const string& host, int port) :
    m_host(host), m_port(port)
{
    m_socket = new QTcpSocket(this);
    m_socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
}

bool TcpClient::start()
{
    m_socket->connectToHost(QHostAddress(QString::fromStdString(m_host)),
                            static_cast<quint16>(m_port));
    return m_socket->waitForConnected();
}

bool TcpClient::stop()
{
    m_socket->disconnectFromHost();
    return true;
}

bool TcpClient::read(string& data)
{
    if (m_socket->state() != QTcpSocket::ConnectedState)
        return false;

    while (m_socket->bytesAvailable() < 4)
    {
        m_socket->waitForReadyRead(-1);
        if (m_socket->state() != QTcpSocket::ConnectedState)
            return false;
    }
    QByteArray tmp = m_socket->read(4);
    qint32 size;
    QDataStream ds(&tmp, QIODevice::ReadWrite);
    ds >> size;

    while (m_socket->bytesAvailable() < size)
    {
        m_socket->waitForReadyRead(-1);
        if (m_socket->state() != QTcpSocket::ConnectedState)
            return false;
    }
    tmp = m_socket->read(size);
    data = string(tmp.data());

    return true;
}

bool TcpClient::write(const string& data)
{
    if (m_socket->state() != QTcpSocket::ConnectedState)
        return false;

    QByteArray array;
    array.append(data.c_str());

    QByteArray temp;
    QDataStream ds(&temp, QIODevice::ReadWrite);
    ds << array.size();
    int ret = m_socket->write(temp);
    if (ret < 0) return false;

    ret = m_socket->write(array);
    if (ret < 0) return false;

    return m_socket->waitForBytesWritten(-1);
}
