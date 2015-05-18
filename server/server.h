#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <QObject>
#include <QList>
#include <QTcpServer>

//Exception émise quand le client ne répond pas à temps
struct ServerTimeoutException : public std::runtime_error
{
    ServerTimeoutException(const std::string& what) :
        runtime_error(what) {}
};

class Server
{
public:
    Server();
    virtual ~Server();
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual void waitUntilReady() = 0;

    virtual bool read(int client, std::string& data, int timeoutMs=2000) = 0;
    virtual bool write(int client, const std::string& data, int timeoutMs=2000) = 0;
};

class TcpServer : public QObject, public Server
{
    Q_OBJECT
public:
    TcpServer(const std::string& host, int port);

    bool start();
    bool stop();
    void waitUntilReady();

    bool read(int client, std::string& data, int timeoutMs=2000);
    bool write(int client, const std::string& data, int timeoutMs=2000);

private:
    QTcpServer* m_server;
    QList<QTcpSocket*> m_clients;
    std::string m_host;
    int m_port;
};

#endif // SERVER_H
