#ifndef MAP_H
#define MAP_H

#include <map>
#include <vector>
#include "aliens/alien.h"

class Gui;
class Map
{
public:
    Map(int width=0, int height=0, Gui* gui=0);
    ~Map();

    //Accesseur et mutateurs de largeur et hauteur
    //Ne devrait pas être négatifs
    void setWidth(int width);
    int width() const;

    void setHeight(int height);
    int height() const;

    //Indique quand la map est pleine : ca ne devrait jamais arriver !
    bool full() const;

    //retourne un pointeur vers l'alien à la position (x,y) ou
    //le pointeur nul si aucun alien à cette case
    Alien* operator()(int x, int y) const;

    //retourne une paire (x,y) indiquant la position de l'alien
    //dans la carte ou la paire (-1,-1) si l'alien n'existe pas
    std::pair<int,int> operator[](Alien* alien) const;

    //retourne les voisins d'un alien (utilise la règle de la visibilité)
    std::vector<Alien*> neighboors(Alien* alien) const;

    //retourne la nourriture autour d'un alien (utilise la règle de la visibilité)
    std::vector< std::pair<int,int> > foods(Alien* alien) const;

    //retourne une liste de tous les aliens de la carte
    std::vector<Alien*> aliens() const;

    bool foodAt(int x, int y) const;

    void addFood(int x, int y);
    void addAlien(Alien* alien, int x, int y);

    void removeFood(int x, int y);
    void removeAlien(Alien* alien);

    void moveAlien(Alien* alien, int x, int y);

    int countAliens(Alien::Species species) const;
    int countFood() const;

    std::pair<int, int> randomEmptySpot() const;

    int pairToPos(int x, int y) const;
    void posToPair(int pos, int &x, int &y) const;

private:
    int m_width;
    int m_height;

    //La nourriture est stockée dans un tableau 1D de booléen
    std::vector<bool> m_food;

    //Pour être efficace, les aliens sont stockés dans deux tables
    //Une table permet de trouver la position d'un alien en fonction de son pointeur
    //L'autre fait l'inverse et permet de vérifier si un alien est présent à une case
    //Mettez-à jour ces deux tables lorsque vos fonctions doivent modifier la carte
    std::map<Alien*, int> m_aliensToPos;
    std::map<int, Alien*> m_posToAliens;

    Gui* m_gui;
};

#endif // MAP_H
