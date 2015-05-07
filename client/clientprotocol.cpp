#include "client.h"
#include "clientprotocol.h"

ClientProtocol::ClientProtocol(Client* client) :
    m_client(client)
{
    m_client->start();
}

ClientProtocol::~ClientProtocol()
{
    m_client->stop();
}

bool ClientProtocol::exec()
{
    return true;
}

