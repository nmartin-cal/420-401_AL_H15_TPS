#include "aliens.h"

Alien::Attack AlienYuhq::fight(int alienId, Alien::Color alienColor, Alien::Species alienSpecies)
{
    return Forfeit;
}

Alien::Move AlienYuhq::move()
{
    return None;
}

bool AlienYuhq::eat()
{
    return false;
}

Alien::Color AlienYuhq::color()
{
    return Gray;
}

Alien::Species AlienYuhq::species()
{
    return Yuhq;
}