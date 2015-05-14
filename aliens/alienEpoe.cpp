#include "aliens.h"

//TODO : Implémenter les comportements prédéfinis de ces aliens
//tel que décris dans l'énoncé.

using namespace std;

AlienEpoe::AlienEpoe() : Alien(Epoe)
{

}


Alien::Attack AlienEpoe::fight(Color alienColor, Species alienSpecies)
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