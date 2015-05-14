#include <sstream>
#include <iostream>
#include "alien.h"
#include "../src/constants.h"
#include "../server/server.h"

using namespace std;

const string& Alien::moveString(Alien::Move move)
{
    static string moveStr[]  =
    {
        "aucun", "haut", "gauche", "bas", "droit"
    };
    static string empty = "";

    if (move >= None && move <= Right)
        return moveStr[move];
    return empty;
}

const string& Alien::colorString(Alien::Color col)
{
    static string colorStr[]  =
    {
        "bleu clair", "bleu foncé", "gris", "jaune", "rouge", "vert", "violet"
    };
    static string empty = "";

    if (col >= LightBlue && col <= Purple)
        return colorStr[col];
    return empty;
}

const string& Alien::speciesString(Alien::Species sp)
{
    static string speciesStr[]  =
    {
        "Uquoma", "Og", "Yuhq", "Epoe", "Grutub", "Owa"
    };
    static string empty = "";

    if (sp >= Uqomua && sp <= Owa)
        return speciesStr[sp];
    return empty;
}

const string& Alien::attackString(Alien::Attack attack)
{
    static string attackStr[]  =
    {
        "Plasma", "Acide", "Spore"
    };
    static string empty = "";

    if (attack >= Plasma && attack <= Fungus)
        return attackStr[attack];
    return empty;
}

int Alien::m_ids = 0;

Alien::Alien(Species species) :
    m_realSpecies(species),
    m_currentTurnSpecies(species), //va changer éventuellement !
    m_currentTurnColor(LightBlue), //va changer éventuellement !
    m_energy(Constants::MaxEnergy),
    m_mating(false),
    m_hasMated(false),
    m_eating(false)
{
    m_id = m_ids;
    m_ids = m_id+1;
}

Alien::Alien(int id) :
    m_realSpecies(Grutub), //ignoré de toute façons
    m_currentTurnSpecies(Grutub), //va changer éventuellement !
    m_currentTurnColor(LightBlue), //va changer éventuellement !
    m_energy(Constants::MaxEnergy),
    m_mating(false),
    m_hasMated(false),
    m_eating(false),
    m_id(id)
{
}

Alien::~Alien()
{
}

int Alien::id() const
{
    return m_id;
}

Alien::Species Alien::realSpecies() const
{
    return m_realSpecies;
}

Alien::Species Alien::currentTurnSpecies() const
{
    return m_currentTurnSpecies;
}

Alien::Color Alien::currentTurnColor() const
{
    return m_currentTurnColor;
}

int Alien::energy() const
{
    return m_energy;
}

bool Alien::sleeping() const
{
    return m_energy == 0;
}

int Alien::sleepingTurn() const
{
    if (!sleeping())
        return -1;
    return m_sleepingTurn;
}

void Alien::setSleepingTurn(int turn)
{
    m_sleepingTurn = turn;
}

void Alien::setHasMated(bool hasMated)
{
    m_hasMated = hasMated;
}

bool Alien::hasMated() const
{
   return m_hasMated;
}

bool Alien::mating() const
{
    return m_mating;
}

void Alien::setMating(bool mating)
{
    m_mating = mating;
}

void Alien::setEatingTurn(int turn)
{
    m_eatingTurn = turn;
}

int Alien::matingTurn() const
{
    if (!mating())
        return -1;
    return m_matingTurn;
}

void Alien::setMatingTurn(int turn)
{
    m_matingTurn = turn;
}

bool Alien::eating() const
{
    return m_eating;
}

void Alien::setEating(bool eating)
{
    m_eating = eating;
}

void Alien::setCurrentTurnSpecies(Species species)
{
    m_currentTurnSpecies = species;
}

void Alien::setCurrentTurnColor(Alien::Color col)
{
    m_currentTurnColor = col;
}

int Alien::eatingTurn() const
{
    if (!eating())
        return -1;
    return m_eatingTurn;
}

void Alien::addEnergy(int delta)
{
    m_energy = std::min(m_energy+delta, Constants::MaxEnergy);
}

void Alien::removeEnergy(int delta)
{
    m_energy = std::max(m_energy-delta, 0);
}

void Alien::setEnergy(int energy)
{
    m_energy = std::min(std::max(energy, 0), Constants::MaxEnergy);
}


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
    if (a < Alien::Plasma || a > Alien::Fungus)
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
