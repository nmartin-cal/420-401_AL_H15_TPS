#ifndef GAME_H
#define GAME_H

#include <stdexcept>
#include "map.h"
#include "gui.h"

struct GameException : public std::runtime_error
{
    GameException(const std::string& what) :
        std::runtime_error(what) {}
};

class Server;
class Game
{
public:
    Game(Server* server,
         int width, int height,
         int numOfEachSpecies=25,
         bool localPlayersOnly=false);
    void update();
    bool simulationOver();

private:
    void updateStats();
    void updateGameStatus();
    bool updateAlien(Alien* alien);
    void sendPlayerInfo(int clientId);
    void sendMapInfo(int clientId);
    void sendNumAliens(int clientId);
    void sendAlienId(int clientId, int alienId);

private:
    Server* m_server;
    std::vector<Alien*> m_currentAlienPerm;
    int m_tick;
    int m_turn;
    Map m_map;
    Gui m_gui;
    int m_numOfEachSpecies;
    bool m_simulationOver;
};

#endif // GAME_H
