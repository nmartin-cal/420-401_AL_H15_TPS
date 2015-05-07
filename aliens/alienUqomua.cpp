#include "aliens.h"

Alien::Attack AlienUqomua::fight(int alienId, Alien::Color alienColor, Alien::Species alienSpecies)
{
   return Forfeit;
}

Alien::Move AlienUqomua::move()
{
    return None;
}

bool AlienUqomua::eat()
{
    return false;
}

Alien::Color AlienUqomua::color()
{
    return Red;
}

Alien::Species AlienUqomua::species()
{
    return Uqomua;
}