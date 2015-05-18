#ifndef ALIEN_REMOTE_H
#define ALIEN_REMOTE_H

#include <stdexcept>
#include <string>
#include "alien.h"

//Exception émise quand le client n'a pas bien fait sa job !
struct RemotelyControledAlienException : public std::runtime_error
{
    RemotelyControledAlienException(const std::string& what) :
        runtime_error(what) {}
};

class Server;
//Un alien controlé à distance par un client !
//Les méthode réimplémentées ne font qu'attendre les commandes du client
//et émettent des exceptions si un problème se produit
class RemotelyControledAlien : public Alien
{
    public:
        RemotelyControledAlien(Server* serveur, int clientId);
        Alien* clone() const;
        Attack fight(Color alienColor, Species alienSpecies);
        Move move();
        bool eat();
        Color color();
        Species species();
        void position(int x, int y);
        void neighboor(int x, int y, Color color, Species species,
                               bool sleeping, bool mating, bool eating);
        void food(int x, int y);
        void sleep();
        void wakeup();
        void mate();
        void win();
        void lose();
        void winSimulation();
        void loseSimulation();

    private:
        //helpers ..
        int sendAndReceiveCommand(const std::string& cmd, const std::string& data=std::string());
        void sendCommand(const std::string& cmd, const std::string& data=std::string());

    private:
        Server* m_server;
        int m_clientId;
};

#endif // ALIEN_REMOTE_H
