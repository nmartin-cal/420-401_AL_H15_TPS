#include "aliens.h"

//TODO : Implémenter les comportements prédéfinis de ces aliens
//tel que décris dans l'énoncé.

using namespace std;

AlienYuhq::AlienYuhq() : Alien(Yuhq)
{

}

Alien::Attack AlienYuhq::fight(Color alienColor, Species alienSpecies)
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
