#include <algorithm>
#include <sstream>
#include "src/game.h"
#include "src/constants.h"
#include "server/server.h"
#include "aliens/aliens.h"
#include "aliens/alienRemote.h"

#include <iostream>

using namespace std;

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

Game::Game(Server* server, int width, int height, int numOfEachSpecies, bool localPlayersOnly) :
    m_server(server),
    m_gui(width, height),
    m_map(width, height, &m_gui),
    m_tick(-1),
    m_turn(0),
    m_numOfEachSpecies(numOfEachSpecies),
    m_simulationOver(false)
{
    m_gui.show();
    int numCells = width*height;

    for (int i=0; i<Constants::NumPlayers; ++i)
    {
        sendPlayerInfo(i);
        sendMapInfo(i);
        sendNumAliens(i);
    }

    //Placement des aliens
    vector<int> pos(numCells);
    for (int i=0; i<numCells; ++i) pos[i] = i;
    random_shuffle(pos.begin(), pos.end());

    //On devraît utiliser une factory ici un jour ...
    int i=0;
    for (int j=0; j<m_numOfEachSpecies; ++j, ++i)
    {
        Alien* alien = new AlienUqomua();
        m_map.addAlien(alien, pos[i]%width, pos[i]/width);
    }

    for (int j=0; j<m_numOfEachSpecies; ++j, ++i)
    {
        Alien* alien = new AlienOg();
        m_map.addAlien(alien, pos[i]%width, pos[i]/width);
    }

    for (int j=0; j<m_numOfEachSpecies; ++j, ++i)
    {
        Alien* alien = new AlienYuhq();
        m_map.addAlien(alien, pos[i]%width, pos[i]/width);
    }

    for (int j=0; j<m_numOfEachSpecies; ++j, ++i)
    {
        Alien* alien = new AlienEpoe();
        m_map.addAlien(alien, pos[i]%width, pos[i]/width);
    }

    if (!localPlayersOnly)
    {
        for (int k=0; k<Constants::NumPlayers; ++k)
        {
            for (int j=0; j<m_numOfEachSpecies; ++j, ++i)
            {
                Alien* alien = new RemotelyControledAlien(m_server, k);
                m_map.addAlien(alien, pos[i]%width, pos[i]/width);
                sendAlienId(k, alien->id());
            }
        }
    } else {
        //instancie uniquement des Grutub en local, les Owa ne sont pas là :)
        for (int j=0; j<m_numOfEachSpecies; ++j, ++i)
        {
            //TODO! Décommenter les lignes suivantes quand votre SmartAlien
            //sera implémenté pour que le simulateur instancie votre espèce
            //en mode local!

            /*
            //on fixe les ids des aliens intelligents à 200, 201, 202...
            //ca n'a pas d'importance en local de toutes façons
            Alien* alien = new SmartAlien(200+j);
            m_map.addAlien(alien, pos[i]%width, pos[i]/width);
            */
        }
    }

    //Placement de la bouffe !
    int numOfFoods = static_cast<int>(Constants::PercentageOfFood * numCells / 100. + 0.5);
    for (int j=0; j<numOfFoods; ++j, ++i)
    {
        m_map.addFood(pos[i]%width, pos[i]/width);
    }
    updateStats();
}

void Game::update()
{
    Gui::UpdateType type = m_gui.updateType();
    bool debug = m_gui.debug();

    if (m_tick == -1)
    {
        if (debug)
        {
            ostringstream oss;
            oss << "================" << endl;
            oss << "Tour " << m_turn << endl;
            m_gui.appendDebug(oss.str());
        }

        //Remet de la bouffe
        if (m_turn % Constants::FoodRespawnTurns == 0)
        {
            if (m_map.full())
                throw GameException("La carte est pleine!");
            pair<int, int> p = m_map.randomEmptySpot();
            m_map.addFood(p.first, p.second);
            if (debug)
            {
                ostringstream oss;
                oss << "Nouvelle nourriture a la case : (" << p.first << "," << p.second << ")";
                m_gui.appendDebug(oss.str());
            }
        }

        m_currentAlienPerm = m_map.aliens();
        int nbs = m_currentAlienPerm.size();

        //Va chercher la couleur et l'espèce de chaque alien pour ce tour-ci !
        //et met à jour les champs locaux
        //Utilise une boucle for plutot que des itérateurs pour éviter tout
        //bug lié à l'invalidation des itérateurs...
        for (int i=0; i<nbs; ++i)
        {
            Alien* curAlien = m_currentAlienPerm[i];
            if (!curAlien) continue;

            //trouve la position de l'alien !
            pair<int,int> pos = m_map[curAlien];
            int x = pos.first;
            int y = pos.second;
            //peut arriver si l'alien s'est fait disqualifié !
            if (x<0 || y < 0) continue;

            //pas de choix pour les aliens qui dorment !
            if (!curAlien->sleeping() &&
                !curAlien->eating() &&
                !curAlien->mating())
            {

                try {
                    Alien::Color col = curAlien->color();
                    curAlien->setCurrentTurnColor(col);

                    Alien::Species species = curAlien->species();
                    curAlien->setCurrentTurnSpecies(species);

                    if (debug)
                    {
                        ostringstream oss;
                        oss << "Alien " << curAlien->id() << " a choisi l'espèce "
                            << Alien::speciesString(species) << " et la couleur "
                            << Alien::colorString(col);
                        m_gui.appendDebug(oss.str());
                        updateStats();
                        updateGameStatus();
                    }
                } catch (runtime_error& e)
                {
                    disqualify(curAlien);
                    continue;
                }
            }

            //juste pour updater couleur et espece !
            m_gui.moveAlien(curAlien, x, y);
        }

        //Choisit un ordre d'update des aliens
        random_shuffle(m_currentAlienPerm.begin(), m_currentAlienPerm.end());

        m_tick++;
    }

    if (type != Gui::NONE)
    {
        int nbs = m_currentAlienPerm.size();
        if (type == Gui::TICK)
        {
            bool ret = false;
            while (!ret && m_tick < nbs)
            {
                try {
                    ret = updateAlien(m_currentAlienPerm[m_tick]);
                } catch (runtime_error& e)
                {
                    disqualify(m_currentAlienPerm[m_tick]);
                }
                m_tick++;
            }
            if (m_tick == nbs)
            {
                if (debug)
                {
                    ostringstream oss;
                    oss << "Fin du tour " << m_turn;
                    m_gui.appendDebug(oss.str());
                }
                m_turn++;
                m_tick = -1;
                updateStats();
                updateGameStatus();
            }
            //on a fait un tick !
            m_gui.setUpdateType(Gui::NONE);
        } else
        {
            //on finit le tour !
            while (m_tick < nbs)
            {
                try {
                    updateAlien(m_currentAlienPerm[m_tick]);
                } catch (runtime_error& e)
                {
                    disqualify(m_currentAlienPerm[m_tick]);
                }
                m_tick++;
            }
            if (debug)
            {
                ostringstream oss;
                oss << "Fin du tour " << m_turn;
                m_gui.appendDebug(oss.str());
            }
            m_turn++;
            m_tick = -1;
            updateStats();
            updateGameStatus();
            if (type == Gui::TURN)
            {
                //on a fait un tour !
                m_gui.setUpdateType(Gui::NONE);
            }
        }
    }

    if (type != Gui::ALL) m_gui.update(10);
    else m_gui.update(m_gui.updateTime());
}

bool Game::simulationOver()
{
    return !m_gui.isVisible() || m_simulationOver;
}

void Game::updateStats()
{
    ostringstream oss;
    oss << "Tour <b>" << m_turn << "</b><br>";
    oss << "Nourriture  <b>" << m_map.countFood() << "</b>";
    oss << "<table>";
    oss << "<thead><tr>";
    for (int i=Alien::Uqomua; i<=Alien::Owa; ++i)
    {
        Alien::Species sp = static_cast<Alien::Species>(i);
        oss << "<td width='"<< int(100.0/Constants::NumSpecies) << "%'>" << Alien::speciesString(sp) << "</td>";
    }
    oss << "</tr></thead>";
    oss << "<tbody><tr>";
    for (int i=Alien::Uqomua; i<=Alien::Owa; ++i)
    {
        Alien::Species sp = static_cast<Alien::Species>(i);
        oss << "<td><b>" << m_map.countAliens(sp) << "</b></td>";
    }
    oss << "</tr></tbody>";
    oss << "</table>";
    m_gui.setStats(oss.str());
}

void Game::updateGameStatus()
{
    int numCells = m_map.width()*m_map.height();
    vector<int> alienCounts(Constants::NumSpecies);
    for (int i=Alien::Uqomua; i<=Alien::Owa; ++i)
    {
        Alien::Species sp = static_cast<Alien::Species>(i);
        alienCounts[i] = m_map.countAliens(sp);
    }

    int speciesWinner = -1;
    for (int i=0; i<Constants::NumSpecies; ++i)
        if (alienCounts[i] / double(numCells) > 0.5)
        {
            //cet équipe a gagné
            speciesWinner = i;
            break;
        }

    if (speciesWinner < 0)
    {
        if (m_turn >= Constants::MaxTurns)
        {
            int maxCount = -1;
            for (int i=0; i<Constants::NumSpecies; ++i)
            {
                if (alienCounts[i] > maxCount)
                {
                    //cet équipe a gagné
                    speciesWinner = i;
                    maxCount = alienCounts[i];
                }
            }
        }
    }

    if (speciesWinner >= 0)
    {
        //prévient les gagants et perdants
        vector<Alien*> aliens = m_map.aliens();
        int totalCount = aliens.size();
        for (int i=0; i<totalCount; ++i)
        {
            if (aliens[i]->realSpecies() == i)
                aliens[i]->winSimulation();
            else
                aliens[i]->loseSimulation();
        }

        ostringstream oss;
        oss << "La simulation s'est terminée par la victoire des "
            << Alien::speciesString(static_cast<Alien::Species>(
                                        speciesWinner)) << "!";
        m_gui.simulationOver(oss.str());
        m_simulationOver = true;
    }
}

bool Game::updateAlien(Alien* alien)
{
    bool debug = m_gui.debug();
    if (debug)
    {
        ostringstream oss;
        oss << "-----" << endl;
        oss << "Mise à jour de l'alien " << alien->id()
            << " d'énergie " << alien->energy();
        m_gui.appendDebug(oss.str());
    }
    if (!alien) return false;
    //trouve la position de l'alien !
    pair<int,int> pos = m_map[alien];
    int x = pos.first;
    int y = pos.second;
    //peut arriver si l'alien s'est fait tué !
    if (x<0 || y < 0) return false;

    //devrait jamais arriver
    if (x >= m_map.width() || y >= m_map.height())
        throw GameException("Coordonnées d'un alien incohérentes!");

    //est-ce que l'alien dort ?
    if (alien->sleeping())
    {
        //compte le nombre de tour écoulé depuis qu'il dort !
        int dturn = m_turn - alien->sleepingTurn();
        if (dturn >= Constants::SleepingTurnsIfTired)
        {
            if (debug)
            {
                ostringstream oss;
                oss << "L'alien se réveille";
                m_gui.appendDebug(oss.str());
            }
            //réveille l'alien, il a assez dormi
            alien->setEnergy(Constants::MaxEnergy);
            alien->wakeup();
        }
        else
        {
            if (debug)
            {
                ostringstream oss;
                oss << "L'alien doit encore dormir "
                    << (Constants::SleepingTurnsIfTired-dturn) << " tours";
                m_gui.appendDebug(oss.str());
            }
            //rien à faire ! l'alien dort
            return true;
        }
    }

    //est-ce qu'il s'accouple
    if (alien->mating())
    {
        int dturn = m_turn - alien->matingTurn();
        if (dturn >= Constants::SleepingTurnsIfMating)
        {
            if (debug)
            {
                ostringstream oss;
                oss << "L'alien vient de finir de s'accoupler";
                m_gui.appendDebug(oss.str());
            }
            //réveille l'alien, il a assez dormi
            alien->setMating(false);
            alien->wakeup();

            //il faut faire apparaître un bébé !!
            if (random()%2 == 0)
            {
                // 0 bébé 5%
                // 1 bébé 85%
                // 2 bébés 10%
                int r = random()%100;
                if (r < 5)
                {
                    //0 bébé : pas de chances !
                    if (debug)
                    {
                        ostringstream oss;
                        oss << "Malheuresement, aucun bébé n'a été créé!";
                        m_gui.appendDebug(oss.str());
                    }
                } else if (r < 5+85)
                {
                    //1 bébé au hasard !
                    if (m_map.full())
                        throw GameException("La carte est pleine!");
                    pair<int, int> p = m_map.randomEmptySpot();
                    m_map.addAlien(alien->clone(), p.first, p.second);
                    if (debug)
                    {
                        ostringstream oss;
                        oss << "Un bébé n'a été créé à la case (" << p.first
                            << "," << p.second << ")";
                        m_gui.appendDebug(oss.str());
                    }
                }
                //else : on applique les probabilités sur le conjoint
                //de temps en temps ça pourra générer 4 bébés ... mais en moyenne
                //cela fera exactement ce que l'on veut !
            } else
            {
                //2 bébés au hasard !
                for (int k=0; k<2; ++k)
                {
                    if (m_map.full())
                        throw GameException("La carte est pleine!");
                    pair<int, int> p = m_map.randomEmptySpot();
                    m_map.addAlien(alien->clone(), p.first, p.second);
                    if (debug)
                    {
                        ostringstream oss;
                        oss << "Un " << (k==0 ? "premier" : "second")
                            << " a été créé à la case (" << p.first
                            << "," << p.second << ")";
                        m_gui.appendDebug(oss.str());
                    }
                }
            }
        } else
        {
            if (debug)
            {
                ostringstream oss;
                oss << "L'alien doit encore se reposer "
                    << (Constants::SleepingTurnsIfMating-dturn) << " tours";
                m_gui.appendDebug(oss.str());
            }
            //rien à faire ! l'alien dort
            return true;
        }
    }

    //est-ce qu'il mange ?
    if (alien->eating())
    {
        //compte le nombre de tour écoulé depuis qu'il dort !
        int dturn = m_turn - alien->eatingTurn();
        if (dturn >= Constants::SleepingTurnsIfEating)
        {
            if (debug)
            {
                ostringstream oss;
                oss << "L'alien vient de finir de manger";
                m_gui.appendDebug(oss.str());
            }
            //réveille l'alien, il a assez dormi
            alien->setEating(false);
            alien->wakeup();
        } else
        {
            //rien à faire, l'alien digère
            if (debug)
            {
                ostringstream oss;
                oss << "L'alien doit encore digérer "
                    << (Constants::SleepingTurnsIfEating-dturn) << " tours";
                m_gui.appendDebug(oss.str());
            }
            return true;
        }
    }

    //envoit les coordonnées à l'alien !
    alien->position(x, y);
    if (debug)
    {
        ostringstream oss;
        oss << "Position de l'alien (" << x << "," << y << ")";
        m_gui.appendDebug(oss.str());
    }

    //envoit les voisins à l'alien !
    vector<Alien*> aliens = m_map.neighboors(alien);
    for (vector<Alien*>::iterator it = aliens.begin(), itE = aliens.end();
         it != itE; ++it)
    {
        Alien* nalien = *it;
        pair<int,int> pos = m_map[nalien];
        alien->neighboor(pos.first, pos.second,
                         nalien->currentTurnColor(),
                         nalien->currentTurnSpecies(),
                         nalien->sleeping(),
                         nalien->mating(),
                         nalien->eating());
        if (debug)
        {
            ostringstream oss;
            oss << "Voit un voisin alien d'id " << nalien->id()
                << " à la position (" << pos.first << "," << pos.second << ") "
                << " de couleur " << nalien->currentTurnColor()
                << " d'espèce " << nalien->currentTurnSpecies()
                << " qui " << (nalien->sleeping() ? "dort" : "ne dort pas") << ", "
                << (nalien->mating() ? "se reproduit" : "ne se reproduit pas") << ", "
                << (nalien->eating() ? "mange" : "ne mange pas");
            m_gui.appendDebug(oss.str());
        }
    }

    //envoit la bouffe à l'alien
    vector< pair<int,int> > foods = m_map.foods(alien);
    for (vector< pair<int,int> >::iterator it = foods.begin(), itE = foods.end();
         it != itE; ++it)
    {
        pair<int,int> food = *it;
        alien->food(food.first, food.second);
        if (debug)
        {
            ostringstream oss;
            oss << "Voit de la nourriture "
                << " à la position (" << food.first << "," << food.second << ")";
            m_gui.appendDebug(oss.str());
        }
    }

    //ou l'alien veut aller ?
    Alien::Move move = alien->move();
    if (debug)
    {
        ostringstream oss;
        oss << "Choisit le mouvement " << Alien::moveString(move);
        m_gui.appendDebug(oss.str());
    }

    int nx, ny;
    switch (move)
    {
    case Alien::Up:
        nx = x; ny = mod(y-1, m_map.height());
        break;
    case Alien::Down:
        nx = x; ny = mod(y+1, m_map.height());
        break;
    case Alien::Left:
        nx = mod(x-1, m_map.width()); ny = y;
        break;
    case Alien::Right:
        nx = mod(x+1, m_map.width()); ny = y;
        break;
    case Alien::None:
        //l'alien ne veut pas bouger, il perd 1 énergie !
        alien->removeEnergy(Constants::NoMovePenalty);
        if (debug)
        {
            ostringstream oss;
            oss << "Ne bouge pas et perd " << Constants::NoMovePenalty << " énergie";
            m_gui.appendDebug(oss.str());
        }
        if (alien->energy() == 0)
        {
            if (debug)
            {
                ostringstream oss;
                oss << "N'a plus d'énergie et doit dormir";
                m_gui.appendDebug(oss.str());
            }
            //plus d'énergie .. fait dormir l'alien
            alien->setSleepingTurn(m_turn);
            alien->sleep();
        }
        return true;
    }

    //qu'est-ce qu'il y a sur la case ?
    if (m_map.foodAt(nx, ny))
    {
        //est-ce que l'alien veut manger ?
        if (alien->eat())
        {
            if (debug)
            {
                ostringstream oss;
                oss << "Mange et gagne " << Constants::EatingBonus << " énergie mais doit dormir";
                m_gui.appendDebug(oss.str());
            }

            alien->addEnergy(Constants::EatingBonus);
            alien->setEatingTurn(m_turn);
            alien->setEating(true);

            //l'alien mange !
            m_map.removeFood(nx, ny);
            m_map.moveAlien(alien, nx, ny);
            return true;
        }
    }

    Alien* otherAlien = m_map(nx, ny);
    if (otherAlien)
    {
        //un autre alien !
        if (otherAlien->realSpecies() == alien->realSpecies())
        {
            //meme espèce !
            if (!alien->hasMated() &&
                !otherAlien->hasMated() &&
                !otherAlien->mating() &&
                !otherAlien->sleeping() &&
                !otherAlien->eating())
            {
                //partenaire trouve !
                if (debug)
                {
                    ostringstream oss;
                    oss << "S'accouple avec l'alien d'id " << otherAlien->id();
                    m_gui.appendDebug(oss.str());
                }

                //notifie les aliens qu'ils s'accouplent !
                alien->mate();
                alien->setMating(true);
                alien->setMatingTurn(m_turn);
                alien->setHasMated(true);

                otherAlien->mate();
                otherAlien->setMating(true);
                otherAlien->setMatingTurn(m_turn);
                otherAlien->setHasMated(true);

                //fini !
                return true;
            } else
            {
                //impossible de se reproduire !
                //on ne peut pas aller sur la case,
                //le tour est perdu !
                alien->removeEnergy(Constants::NoMovePenalty);
                if (debug)
                {
                    ostringstream oss;
                    oss << "Ne peut s'accoupler et perd " << Constants::NoMovePenalty << " énergie";
                    m_gui.appendDebug(oss.str());
                }
                if (alien->energy() == 0)
                {
                    //plus d'énergie .. fait dormir l'alien
                    if (debug)
                    {
                        ostringstream oss;
                        oss << "N'a plus d'énergie et doit dormir";
                        m_gui.appendDebug(oss.str());
                    }
                    alien->setSleepingTurn(m_turn);
                    alien->sleep();
                }
                return true;
            }
        } else {
            //espèce différente !!
            //si l'autre dort d'une quelconque manière, il perd instantannément
            if (otherAlien->sleeping() || otherAlien->mating() || otherAlien->eating())
            {
                if (debug)
                {
                    ostringstream oss;
                    oss << "Se bat contre l'alien d'id " << otherAlien->id()
                        << " qui ne peut pas combattre et gagne automatiquement";
                    m_gui.appendDebug(oss.str());
                }
                alien->win();
                otherAlien->lose();
                m_map.removeAlien(otherAlien);
            } else {
                if (debug)
                {
                    ostringstream oss;
                    oss << "Combat l'alien d'id " << otherAlien->id();
                    m_gui.appendDebug(oss.str());
                }
                //COMBAT !!
                Alien::Attack a1 = alien->fight(otherAlien->m_currentTurnColor,
                                                otherAlien->m_currentTurnSpecies);
                Alien::Attack a2 = otherAlien->fight(alien->m_currentTurnColor,
                                                     alien->m_currentTurnSpecies);
                char table[] = " 011 001 ";
                bool win1;
                if (a1 == a2) win1 = random() % 2;
                else if (a1 == Alien::Forfeit) win1 = false;
                else if (a2 == Alien::Forfeit) win1 = true;
                else win1 = static_cast<bool>(table[a2*3+a1]-'0');

                if (debug)
                {
                    ostringstream oss;
                    oss << "Alien #1 choisit " << Alien::attackString(a1)
                        << " et alien #2 choisit " << Alien::attackString(a2);
                    m_gui.appendDebug(oss.str());
                }

                if (win1)
                {
                    if (debug)
                    {
                        ostringstream oss;
                        oss << "Gagne le combat !";
                        m_gui.appendDebug(oss.str());
                    }
                    alien->win();
                    otherAlien->lose();
                    m_map.removeAlien(otherAlien);
                } else
                {
                    if (debug)
                    {
                        ostringstream oss;
                        oss << "Perd le combat et meurt dans d'atroces souffrances !";
                        m_gui.appendDebug(oss.str());
                    }
                    otherAlien->win();
                    alien->lose();
                    m_map.removeAlien(alien);
                    return true;
                    //fini !
                }
            }
            //un des deux et enlevé du monde
        }
    }

    //on se déplace gentiment
    m_map.moveAlien(alien, nx, ny);
    alien->removeEnergy(Constants::MovePenalty);
    if (debug)
    {
        ostringstream oss;
        oss << "Perd " << Constants::MovePenalty << " énergies";
        m_gui.appendDebug(oss.str());
    }
    if (alien->energy() == 0)
    {
        if (debug)
        {
            ostringstream oss;
            oss << "N'a plus d'énergie et doit dormir";
            m_gui.appendDebug(oss.str());
        }
        //plus d'énergie .. fait dormir l'alien
        alien->setSleepingTurn(m_turn);
        alien->sleep();
    }

    return true;
}

void Game::sendPlayerInfo(int clientId)
{
    stringstream ss;
    ss << "playerId " << clientId;
    m_server->write(clientId, ss.str());
}

void Game::sendMapInfo(int clientId)
{
    stringstream ss;
    ss << "mapSize " << m_map.width() << " " << m_map.height();
    m_server->write(clientId, ss.str());
}

void Game::sendNumAliens(int clientId)
{
    stringstream ss;
    ss << "numAliens " << m_numOfEachSpecies;
    m_server->write(clientId, ss.str());
}

void Game::sendAlienId(int clientId, int alienId)
{
    stringstream ss;
    ss << "alienId " << alienId;
    m_server->write(clientId, ss.str());
}

void Game::disqualify(Alien* alien)
{
    int nbs = m_currentAlienPerm.size();
    Alien::Species spDisqualified = alien->realSpecies();
    //l'alien a fail ! on élimine l'équipe !!
    for (int i=0; i<nbs; ++i)
    {
        if (m_currentAlienPerm[i]->realSpecies() == spDisqualified)
            m_map.removeAlien(m_currentAlienPerm[i]);
    }

    stringstream ss;
    ss << "L'alien d'id " << alien->id() << " n'a pas donné "
       << "de réponse valide ou dans le temps imparti à l'oracle, ou une erreur s'est produite. Son espèce "
       << Alien::speciesString(spDisqualified) << " a donc été disqualifiée.";
    m_gui.speciesDisqualified(ss.str());
    updateStats();
    updateGameStatus();
}
