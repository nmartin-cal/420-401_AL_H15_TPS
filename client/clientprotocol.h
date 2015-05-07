#ifndef CLIENTPROTOCOL_H
#define CLIENTPROTOCOL_H

//TODO : complétez cette class comme bon vous semble !
class Client;
class ClientProtocol
{
public:
    ClientProtocol(Client* client);
    ~ClientProtocol();
    //Ajoutez tout ce que vous voulez ici !

    bool exec();

private:
    Client* m_client;
};

#endif // CLIENTPROTOCOL_H
