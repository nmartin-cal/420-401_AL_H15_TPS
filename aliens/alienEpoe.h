#ifndef ALIEN_EPOE_H
#define ALIEN_EPOE_H

#include <iostream>
#include "alien.h"

class AlienEpoe : public Alien
{
public:
    AlienEpoe();
    Alien* clone() const { return new AlienEpoe(*this); }
    Attack fight(Color alienColor, Species alienSpecies);
    Move move();
    bool eat();
    Color color();
    Species species();
private:
};

#endif
