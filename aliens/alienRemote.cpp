#include <sstream>
#include "alienRemote.h"
#include "server/server.h"

using namespace std;

/*****************
 * Alien controlé par un client !
 ***************/

RemotelyControledAlien::RemotelyControledAlien(Server* server, int clientId) :
    Alien((clientId == 0 ? Grutub : Owa)),
    m_server(server),
    m_clientId(clientId)
{
}

Alien* RemotelyControledAlien::clone() const
{
    return new RemotelyControledAlien(*this);
}

int RemotelyControledAlien::sendAndReceiveCommand(const std::string& cmd, const std::string& data)
{
    stringstream ss1;
    ss1 << cmd << " " << id() << " " << data;
    if (!m_server->write(m_clientId, ss1.str()))
        throw RemotelyControledAlienException("Erreur de communication avec le client!");

    string tmp;
    if (!m_server->read(m_clientId, tmp))
        throw RemotelyControledAlienException("Erreur de communication avec le client!");

    stringstream ss2(tmp);
    string c;
    int i;
    int val;
    ss2 >> c >> i >> val;
    if (c != cmd)
    {
        stringstream ss;
        ss << "Erreur de commande envoyée par le client dans la réponse,";
        ss << " recu : " << c << " attendu : " << cmd << "." << endl;
        throw RemotelyControledAlienException(ss.str());
    }
    if (i != id())
    {
        stringstream ss;
        ss << "Erreur d'id envoyée par le client dans la réponse,";
        ss << " recu : " << i << " attendu : " << id() << "." << endl;
        throw RemotelyControledAlienException(ss.str());
    }
    return val;
}

Alien::Attack RemotelyControledAlien::fight(Alien::Color alienColor, Alien::Species alienSpecies)
{
    stringstream ss;
    ss << alienColor << " " << alienSpecies;
    int a = sendAndReceiveCommand("fight", ss.str());
    if (a < Alien::Plasma || a > Alien::Forfeit)
        throw RemotelyControledAlienException("Erreur d'attaque envoyée par le client dans la réponse.");
    return static_cast<Alien::Attack>(a);
}

Alien::Move RemotelyControledAlien::move()
{
    int move = sendAndReceiveCommand("move");
    if (move < Alien::None || move > Alien::Right)
        throw RemotelyControledAlienException("Erreur de mouvement envoyé par le client dans la réponse.");
    return static_cast<Alien::Move>(move);
}

bool RemotelyControledAlien::eat()
{
    int e = sendAndReceiveCommand("eat");
    if (e < 0 || e > 1)
        throw RemotelyControledAlienException("Erreur de nourriture envoyée par le client dans la réponse.");
    return static_cast<bool>(e);
}

Alien::Color RemotelyControledAlien::color()
{
    int col = sendAndReceiveCommand("color");
    if (col < Alien::LightBlue || col > Alien::Purple)
        throw RemotelyControledAlienException("Erreur de couleur envoyée par le client dans la réponse.");
    return static_cast<Alien::Color>(col);
}

Alien::Species RemotelyControledAlien::species()
{
    int sp = sendAndReceiveCommand("species");
    if (sp < Alien::Uqomua || sp > Alien::Owa)
        throw RemotelyControledAlienException("Erreur d'espèce envoyée par le client dans la réponse.");
    return static_cast<Alien::Species>(sp);
}

void RemotelyControledAlien::sendCommand(const std::string& cmd, const std::string& data)
{
    stringstream ss1;
    ss1 << cmd << " " << id() << " " << data;
    if (!m_server->write(m_clientId, ss1.str()))
        throw RemotelyControledAlienException("Erreur de communication avec le client!");
}

void RemotelyControledAlien::position(int x, int y)
{
    stringstream ss;
    ss << x << " " << y;
    sendCommand("position", ss.str());
}

void RemotelyControledAlien::neighboor(int x, int y, Alien::Color col, Alien::Species species, bool sleeping, bool mating, bool eating)
{
    stringstream ss;
    ss << x << " " << y << " "
       << col << " " << species << " "
       << static_cast<int>(sleeping) << " "
       << static_cast<int>(mating) << " "
       << static_cast<int>(eating);
    sendCommand("neighboor", ss.str());
}

void RemotelyControledAlien::food(int x, int y)
{
    stringstream ss;
    ss << x << " " << y;
    sendCommand("food", ss.str());
}

void RemotelyControledAlien::sleep()
{
    sendCommand("sleep");
}

void RemotelyControledAlien::wakeup()
{
    sendCommand("wakeup");
}

void RemotelyControledAlien::mate()
{
    sendCommand("mate");
}

void RemotelyControledAlien::win()
{
    sendCommand("win");
}

void RemotelyControledAlien::lose()
{
    sendCommand("lose");
}

void RemotelyControledAlien::winSimulation()
{
    sendCommand("winSimulation");
}

void RemotelyControledAlien::loseSimulation()
{
    sendCommand("loseSimulation");
}
