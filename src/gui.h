#ifndef GUI_H
#define GUI_H

#include <QWidget>
#include <QIcon>
#include "aliens/alien.h"

class QGraphicsScene;
class QGraphicsView;
class QGraphicsItem;
class QPushButton;
class QSlider;
class QLabel;
class QTextEdit;
class QCheckBox;
class Gui : public QWidget
{
    Q_OBJECT
public:
    Gui(int width, int height);

    void addFood(int x, int y);
    void addAlien(Alien* alien, int x, int y);

    void removeFood(int x, int y);
    void removeAlien(Alien* alien);

    void moveAlien(Alien* alien, int x, int y);
    void update(int msecs);

    //pas idéal .. mais bon c'est temporaire !
    enum UpdateType { NONE, TICK, TURN, ALL };
    UpdateType updateType() const;
    void setUpdateType(UpdateType type);
    int updateTime() const;
    bool debug() const;

private:
    void setupImages();

public slots:
    void playOrPause();
    void tick();
    void turn();
    void setSpeed(int);
    void setDebug(bool);
    void appendDebug(const std::string& debug);
    void setStats(const std::string& stats);
    void simulationOver(const std::string& msg);
    void speciesDisqualified(const std::string& msg);

private:
    QGraphicsScene* m_scene;
    QGraphicsView* m_view;
    QPushButton* m_play;
    QPushButton* m_tick;
    QPushButton* m_turn;
    QSlider* m_speed;
    QLabel* m_stats;
    QTextEdit* m_debug;
    QCheckBox* m_debugCb;
    UpdateType m_updateType;
    int m_updateTime;
    bool m_updateDebug;

    QHash<Alien*, QGraphicsItem*> m_aliensToItems;
    QHash<int, QGraphicsItem*> m_foodsToItems;
    QHash< QPair<Alien::Color,Alien::Species>, QIcon> m_alienSprites;
    QIcon m_foodSprite;
    int m_width;
    int m_height;
};

#endif // GUI_H
