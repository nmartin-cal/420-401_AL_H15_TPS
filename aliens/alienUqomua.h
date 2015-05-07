#ifndef ALIEN_UQOMUA_H
#define ALIEN_UQOMUA_H

#include <iostream>
#include "alien.h"

class AlienUqomua : public Alien
{
public:
    AlienUqomua() : Alien(Uqomua) { }
    Alien* clone() const { return new AlienUqomua(*this); }
    Attack fight(int alienId, Color alienColor, Species alienSpecies);
    Move move();
    bool eat();
    Color color();
    Species species();
};

#endif