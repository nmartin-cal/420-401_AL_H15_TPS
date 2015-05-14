#include "aliens.h"

//TODO : Implémenter les comportements prédéfinis de ces aliens
//tel que décris dans l'énoncé.

using namespace std;

AlienUqomua::AlienUqomua() : Alien(Uqomua)
{

}

Alien::Attack AlienUqomua::fight(Color alienColor, Species alienSpecies)
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
