#ifndef ALIEN_UQOMUA_H
#define ALIEN_UQOMUA_H

#include <iostream>
#include "alien.h"

class AlienUqomua : public Alien
{
public:
    AlienUqomua();
    Alien* clone() const { return new AlienUqomua(*this); }
    Attack fight(Color alienColor, Species alienSpecies);
    Move move();
    bool eat();
    Color color();
    Species species();
private:
};

#endif
