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
