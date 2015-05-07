#include "aliens.h"

Alien::Attack AlienOg::fight(int alienId, Alien::Color alienColor, Alien::Species alienSpecies)
{
    return Forfeit;
}

Alien::Move AlienOg::move()
{
    return None;
}

bool AlienOg::eat()
{
    return false;
}

Alien::Color AlienOg::color()
{
    return Purple;
}

Alien::Species AlienOg::species()
{
    return Og;
}