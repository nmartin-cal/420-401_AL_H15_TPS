#include <sstream>
#include <iostream>
#include <map>
#include "client.h"
#include "clientprotocol.h"
#include "../src/map.h"
#include "../aliens/alien.h"

using namespace std;

int main(int argc, char *argv[])
{
    //TODO : ajoutez le support pour les arguments du port et l'host
    string host("127.0.0.1");
    int port  = 9912;
    Client* c = new TcpClient(host, port);

    ClientProtocol protocol(c);
    if (!protocol.exec())
    {
        cerr << "Erreur avec le protocole" << endl;
        return 1;
    }

    return 0;
}

