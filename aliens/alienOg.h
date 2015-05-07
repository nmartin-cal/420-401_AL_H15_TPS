#ifndef ALIEN_OG_H
#define ALIEN_OG_H

#include <iostream>
#include "alien.h"

class AlienOg : public Alien
{
public:
    AlienOg() : Alien(Og) {}
    Alien* clone() const { return new AlienOg(*this); }
    Attack fight(int alienId, Color alienColor, Species alienSpecies);
    Move move();
    bool eat();
    Color color();
    Species species();
};

#endif