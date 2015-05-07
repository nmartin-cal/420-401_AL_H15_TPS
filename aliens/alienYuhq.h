#ifndef ALIEN_YUHQ_H
#define ALIEN_YUHQ_H

#include <iostream>
#include "alien.h"

class AlienYuhq : public Alien
{
public:
    AlienYuhq() : Alien(Yuhq) {}
    Alien* clone() const { return new AlienYuhq(*this); }
    Attack fight(int alienId, Color alienColor, Species alienSpecies);
    Move move();
    bool eat();
    Color color();
    Species species();
};

#endif