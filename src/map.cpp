#include <vector>
#include <cstdlib>
#include <iostream>
#include "gui.h"
#include "map.h"
#include "aliens/alien.h"

using namespace std;

Map::Map(int width, int height, Gui* gui) :
    m_gui(gui)
{
    //TODO! Complétez-moi
}

Map::~Map()
{
    //TODO! Complétez-moi
    //La carte est responsable de détruire les aliens !
}

void Map::setWidth(int width)
{
    //TODO! Complétez-moi
    //Assurez-vous que la largeur fasse du sens
}

int Map::width() const
{
    //TODO! Complétez-moi
    return 0;
}

void Map::setHeight(int height)
{
    //TODO! Complétez-moi
    //Assurez-vous que la largeur fasse du sens
}

int Map::height() const
{
    //TODO! Complétez-moi
    return 0;
}

bool Map::full() const
{
    //TODO! Complétez-moi
    //Regarde si le nombre d'aliens + nombre de nourriture = taille de la carte
    return false;
}

Alien* Map::operator()(int x, int y) const
{
    //TODO! Complétez-moi
    //Retourne l'alien à la case (x,y) ou le pointeur nul le cas échéant
    //Servez-vous de pairToPos !
    return 0;
}

pair<int,int> Map::operator[](Alien* alien) const
{
    //TODO! Complétez-moi
    //Retourne une paire (x,y) représentant la case où l'alien est stocké
    //dans la carte ou la paire (-1, -1) le cas échéant
    //Servez-vous de posToPair !

    return make_pair(-1,-1);
}

vector<Alien*> Map::neighboors(Alien* alien) const
{
    //TODO! Complétez-moi
    //Retourne tous les aliens autour de l'alien passé en paramètre
    //dans un rayon de 3 cases.
    //Utilisez la formule sqrt((x1-x2)^2 + (y1-y2)^2) < 3
    //où l'alien passé en paramètre est à la case (x1,y1) et l'autre alien
    //à la case (x2,y2)

    return vector<Alien*>();
}

vector< pair<int, int> > Map::foods(Alien* alien) const
{
    //TODO! Complétez-moi
    //Retourne toute la nourriture autour de l'alien passé en paramètre
    //dans un rayon de 3 cases.
    //Utilisez la formule sqrt((x1-x2)^2 + (y1-y2)^2) < 3
    //où l'alien passé en paramètre est à la case (x1,y1) et la nourriture
    //à la case (x2,y2)

    return vector< pair<int,int> >();
}

vector<Alien*> Map::aliens() const
{
    //TODO! Complétez-moi
    //Retourne tous les aliens sur la carte (peu importe l'ordre)

    return vector<Alien*>();
}

bool Map::foodAt(int x, int y) const
{
    //TODO! Complétez-moi
    //Retourne vraie si il y a de la nourriture à la case (x,y)
    //faux sinon

    return false;
}

void Map::addFood(int x, int y)
{
    //TODO! Complétez-moi
    //Ajoute de la nourriture dans la carte à la case (x,y)

    //Laissez ce code
    if (m_gui) m_gui->addFood(x, y);
}

void Map::addAlien(Alien* alien, int x, int y)
{
    //TODO! Complétez-moi
    //Ajoute un alien dans la carte à la case (x,y)

    //Laissez ce code
    if (m_gui) m_gui->addAlien(alien, x, y);
}

void Map::removeFood(int x, int y)
{
    //TODO! Complétez-moi
    //Enlève de la nourriture de la case (x,y)

    //Laissez ce code
    if (m_gui) m_gui->removeFood(x, y);
}

void Map::removeAlien(Alien* alien)
{
    //TODO! Complétez-moi
    //Enlève un alien de la case (x,y)

    //Laissez ce code
    if (m_gui) m_gui->removeAlien(alien);
}

void Map::moveAlien(Alien* alien, int x, int y)
{
    //TODO! Complétez-moi
    //Déplace un alien depuis sa case actuelle vers la case (x,y)

    //Laissez ce code
    if (m_gui) m_gui->moveAlien(alien, x, y);
}

int Map::countAliens(Alien::Species species) const
{
    //TODO! Complétez-moi
    //Compte et retourne le nombre d'aliens d'une certaine espèce
    //Vous devez utiliser un algo de la STL et un foncteur qui
    //prend en paramètre l'espèce voulue pour écrire cette fonction

    return 0;
}

int Map::countFood() const
{
    //TODO! Complétez-moi
    //Compte et retourne le nombre d'aliens d'une certaine espèce
    //Vous devez utiliser un algo de la STL pour écrire cette fonction

    return 0;
}

pair<int, int> Map::randomEmptySpot() const
{
    //TODO! Complétez-moi
    //Retourne une case au hasard qui ne contient ni alien
    //ni nourriture.

    return make_pair(0, 0);
}

//Fonction utilitaire pour vous aider !
int Map::pairToPos(int x, int y) const
{
   return y*m_width+x;
}

//Fonction utilitaire pour vous aider !
void Map::posToPair(int pos, int& x, int& y) const
{
   y = pos/m_width;
   x = pos%m_width;
}
