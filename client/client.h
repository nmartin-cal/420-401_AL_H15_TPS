#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <string>

class Client
{
public:
    Client();
    virtual ~Client();

    virtual bool start() = 0;
    virtual bool stop() = 0;

    virtual bool read(std::string& data) = 0;
    virtual bool write(const std::string& data) = 0;
};

class TcpClient : public QObject, public Client
{
    Q_OBJECT
public:
    TcpClient(const std::string& host, int port);

    bool start();
    bool stop();

    bool read(std::string& data);
    bool write(const std::string& data);

private:
    QTcpSocket* m_socket;
    std::string m_host;
    int m_port;
};

#endif // CLIENT_H
