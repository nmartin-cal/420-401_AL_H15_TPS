#ifndef ALIEN_H
#define ALIEN_H

#include <string>

class Alien
{
    public:
        //Les mouvements qu'un alien peut retourner dans la méthode move
        enum Move { None, Up, Left, Down, Right };
        //Convertit un mouvement en chaine pour l'affichage
        static const std::string& moveString(Move move);

        //Les couleurs qu'un alien peut retourner dans la méthode color
        enum Color { LightBlue, DarkBlue, Gray, Yellow, Red, Green, Purple };
        //Convertit une couleur en chaine pour l'affichage
        static const std::string& colorString(Color col);

        //Les espèces qu'un alien peut retourner dans la méthode species
        enum Species { Uqomua, Og, Yuhq, Epoe, Grutub, Owa };
        //Convertit une espèce en chaine pour l'affichage
        static const std::string& speciesString(Species sp);

        //Les attaques qu'un alien peut retourner dans la méthode fight
        enum Attack { Plasma, Acid, Fungus, Forfeit };
        //Convertit une attaque en chaine pour l'affichage
        static const std::string& attackString(Attack attack);

        //Un alien contrôlé localement par le simulateur
        //utiliser ce constructeur pour instancier un alien local
        Alien(Species species);

        //Un alien contrôlé à distance (Grutub ou Owa)
        //utiliser ce constructeur pour instancier un alien dans votre client
        //l'id est obligatoire ! vous controlerez l'alien ayant cet id dans
        //la simulation
        Alien(int id);

        //pour le polymorphisme
        virtual ~Alien();

        //entier unique identifiant chaque agent
        int id() const;

        //retourne la vraie espéce de cet agent (décidée à la création)
        Species realSpecies() const;

        //l'espèce choisit pour ce tour
        Species currentTurnSpecies() const;

        //la couleur choisit pour ce tour
        Color currentTurnColor() const;

        //retourne l'énergie courante
        int energy() const;

        //vrai si l'énergie == 0
        bool sleeping() const;

        //vrai si en train de s'accoupler
        bool mating() const;

        //vrai si s'est déjà accouplé !
        bool hasMated() const;

        //vrai si en train de manger
        bool eating() const;

        //le tour auquel l'agent s'est endormi, -1 si il ne dort pas
        int sleepingTurn() const;

        //le tour auquel l'agent s'est arrété pour manger
        int eatingTurn() const;

        //le tour auquel l'agent s'est arrété pour procréer !
        int matingTurn() const;

        //crée un clone (mais pas avec le même id) !
        virtual Alien* clone() const = 0;

        //appelé pour savoir le déplacement que l'agent veut entreprendre
        virtual Move move() = 0;

        //appelé pour savoir si l'agent veut manger la nourriture sur la case
        //sur laquelle il se trouve
        virtual bool eat() = 0;

        //appelé pour savoir comment l'agent veut attaquer un autre agent
        virtual Attack fight(Color alienColor, Species alienSpecies) = 0;

        //appelé pour savoir la couleur d'un agent
        virtual Color color() = 0;

        //appelé pour savoir l'espèce que l'agent veut fournir aux autres
        virtual Species species() = 0;

        //appelé pour donner la position de cet agent
        virtual void position(int x, int y) {}

        //appelé pour chaque voisin que l'agent voit
        virtual void neighboor(int x, int y, Color color, Species species,
                               bool sleeping, bool mating, bool eating) {}

        //appelé pour chaque nourriture que l'agent voit
        virtual void food(int x, int y) {}

        //appelé lorsque l'agent doit dormir (plus d'énergie)
        virtual void sleep() {}

        //appelé lorsque l'agent se réveille
        virtual void wakeup() {}

        //appelé lorsque l'agent est en train de se reproduire
        virtual void mate() {}

        //appelé lorsque l'agent a gagné un combat
        virtual void win() {}

        //appelé lorsque l'agent a perdu un combat et va mourir
        virtual void lose() {}

        //appelé lorsque l'espèce de l'agent a gagné la simulation
        virtual void winSimulation() {}

        //appelé lorsque l'espèce de l'agent a perdu la simulation
        virtual void loseSimulation() {}

    private:
        //appelable uniqument par Game et Gui!
        void addEnergy(int delta);
        void removeEnergy(int delta);
        void setEnergy(int energy);
        void setSleepingTurn(int turn);

        void setHasMated(bool hasMated);
        void setMatingTurn(int turn);
        void setMating(bool mating);

        void setEatingTurn(int turn);
        void setEating(bool eating);

        void setCurrentTurnSpecies(Species species);
        void setCurrentTurnColor(Color col);

    private:
        //pas accessible des enfants !
        int m_energy;
        int m_sleepingTurn;

        bool m_hasMated;
        bool m_mating;
        int m_matingTurn;

        bool m_eating;
        int m_eatingTurn;

        static int m_ids;
        int m_id;

        Species m_realSpecies;
        Species m_currentTurnSpecies;
        Color m_currentTurnColor;

        friend class Game;
        friend class Gui;
};

#endif // ALIEN_H
