#include "aliens.h"

//TODO : Implémenter les comportements prédéfinis de ces aliens
//tel que décris dans l'énoncé.

using namespace std;

AlienOg::AlienOg() : Alien(Og)
{

}

Alien::Attack AlienOg::fight(Color alienColor, Species alienSpecies)
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
