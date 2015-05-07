#include "aliens.h"

Alien::Attack AlienEpoe::fight(int alienId, Alien::Color alienColor, Alien::Species alienSpecies)
{
    return Forfeit;
}

Alien::Move AlienEpoe::move()
{
    return None;
}

bool AlienEpoe::eat()
{
    return false;
}

Alien::Color AlienEpoe::color()
{
    return Green;
}

Alien::Species AlienEpoe::species()
{
    return Epoe;
}