#ifndef ALIEN_OG_H
#define ALIEN_OG_H

#include <iostream>
#include "alien.h"

class AlienOg : public Alien
{
public:
    AlienOg();
    Alien* clone() const { return new AlienOg(*this); }
    Attack fight(Color alienColor, Species alienSpecies);
    Move move();
    bool eat();
    Color color();
    Species species();
private:
};

#endif
