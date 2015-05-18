#include <QtWidgets>
#include <iostream>
#include "src/constants.h"
#include "aliens/alien.h"
#include "gui.h"

int CellSize;

Gui::Gui(int width, int height) :
    m_width(width), m_height(height),
    m_updateType(NONE),
    m_updateTime(100),
    m_updateDebug(false)
{
    setupImages();
    setWindowTitle(trUtf8("Simulations d'agents autonomes aliens"));

    int screenHeight = qApp->primaryScreen()->size().height() - 100;
    int screenWidth = qApp->primaryScreen()->size().width() - 100;
    double cellHeight = screenHeight / height;
    CellSize = qMin(static_cast<int>(cellHeight), 32);

    QHBoxLayout* hLay = new QHBoxLayout;
    m_scene = new QGraphicsScene;
    m_scene->setSceneRect(0, 0, width*CellSize, height*CellSize);
    m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    m_view = new QGraphicsView;
    m_view->setWindowTitle("Simulation d'aliens");
    m_view->setFixedSize(QSize(width*CellSize+5, height*CellSize+5));
    m_view->setScene(m_scene);
    m_view->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    hLay->addWidget(m_view);

    QVBoxLayout* vSideLay = new QVBoxLayout;
    hLay->addLayout(vSideLay);

    QVBoxLayout* vLay = new QVBoxLayout;
    QGroupBox* groupOptions = new QGroupBox(trUtf8("Options"));
    QHBoxLayout* bLay = new QHBoxLayout;
    m_play = new QPushButton(trUtf8("Continuer"));
    m_play->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_play->setCheckable(true);
    connect(m_play, SIGNAL(toggled(bool)), this, SLOT(playOrPause()));
    m_tick = new QPushButton(trUtf8("Suivant"));
    m_tick->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_tick, SIGNAL(clicked()), this, SLOT(tick()));
    m_turn = new QPushButton(trUtf8("Tour"));
    m_turn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_turn, SIGNAL(clicked()), this, SLOT(turn()));
    bLay->addWidget(m_play);
    bLay->addWidget(m_tick);
    bLay->addWidget(m_turn);
    bLay->addStretch();
    vLay->addLayout(bLay);
    QHBoxLayout* sLay = new QHBoxLayout;
    QLabel* label = new QLabel(trUtf8("Vitesse"));
    m_speed = new QSlider(Qt::Horizontal);
    m_speed->setRange(0, 990);
    m_speed->setValue(1000-100);
    connect(m_speed, SIGNAL(valueChanged(int)), this, SLOT(setSpeed(int)));
    sLay->addWidget(label);
    sLay->addWidget(m_speed);
    sLay->addStretch();
    vLay->addLayout(sLay);
    groupOptions->setLayout(vLay);
    vSideLay->addWidget(groupOptions);

    QVBoxLayout* vLay2 = new QVBoxLayout;
    QGroupBox* groupStats = new QGroupBox(trUtf8("Statistiques"));
    m_stats = new QLabel;
    vLay2->addWidget(m_stats);
    groupStats->setLayout(vLay2);
    vSideLay->addWidget(groupStats);

    QVBoxLayout* vLay3 = new QVBoxLayout;
    QGroupBox* groupDebug = new QGroupBox(trUtf8("Trace"));
    m_debug = new QTextEdit;
    m_debug->setEnabled(false);
    m_debug->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_debugCb = new QCheckBox(trUtf8("Activer la trace"));
    connect(m_debugCb, SIGNAL(toggled(bool)), this, SLOT(setDebug(bool)));
    vLay3->addWidget(m_debugCb);
    vLay3->addWidget(m_debug);
    groupDebug->setLayout(vLay3);
    vSideLay->addWidget(groupDebug);

    setLayout(hLay);
    m_view->show();

    resize(screenWidth, screenHeight);
}

void Gui::addFood(int x, int y)
{
    QFont font = qApp->font();
    font.setPixelSize(CellSize-4);
    QIcon icon = m_foodSprite;
    QGraphicsItem* item = m_scene->addPixmap(icon.pixmap(CellSize, CellSize));
    item->setPos(x*CellSize, y*CellSize);
    m_foodsToItems[y*m_width+x] = item;
}

static const char eatTab[] = {0xf0, 0x9f, 0x8d, 0x95};
static QString eatString = QString::fromUtf8(eatTab, 4);

static const char sleepTab[] = {0xf0, 0x9f, 0x92, 0xa4};
static const QString sleepString = QString::fromUtf8(sleepTab, 4);

static const char mateTab[] = {0xe2, 0x99, 0xa1};
static QString mateString = QString::fromUtf8(mateTab, 3);

static QStringList statusStrings = QStringList() << eatString << sleepString << mateString;

void Gui::addAlien(Alien* alien, int x, int y)
{
    QIcon icon = m_alienSprites[qMakePair(alien->currentTurnColor(), alien->currentTurnSpecies())];
    QGraphicsItem* item = m_scene->addPixmap(icon.pixmap(CellSize, CellSize));
    item->setPos(x*CellSize, y*CellSize);
    QFont font = qApp->font();
    font.setPixelSize(CellSize*0.5);
    font.setBold(true);
    //espece
    QGraphicsItem* text = m_scene->addSimpleText(QString(Alien::speciesString(alien->realSpecies())[0]), font);
    text->setParentItem(item);

    bool statusTab[] = { alien->eating(), alien->sleeping(), alien->mating() };
    //statut
    for (int i=0; i<statusStrings.size(); ++i)
    {
        if (statusTab[i])
        {
            font = QFont("Symbola");
            font.setPixelSize(CellSize*0.4);
            QGraphicsItem* sText = m_scene->addSimpleText(statusStrings[i], font);
            int posX = CellSize - sText->boundingRect().width();
            sText->setParentItem(item);
            sText->setPos(posX, 0);
        }
    }

    m_aliensToItems[alien] = item;
}

void Gui::removeFood(int x, int y)
{
    delete m_foodsToItems[y*m_width+x];
    m_foodsToItems.remove(y*m_width+x);
}

void Gui::removeAlien(Alien* alien)
{
    delete m_aliensToItems[alien];
    m_aliensToItems.remove(alien);
}

void Gui::moveAlien(Alien* alien, int x, int y)
{
    /*
    QGraphicsItem* item = m_aliensToItems.value(alien, 0);
    if (item) item->setPos(x*CellSize, y*CellSize);
    */
    removeAlien(alien);
    addAlien(alien, x, y);
}

void Gui::update(int msecs)
{
    //refresh for specified time !
    QEventLoop loop;
    QTimer::singleShot(msecs, &loop, SLOT(quit()));
    loop.exec();
}

Gui::UpdateType Gui::updateType() const
{
    return m_updateType;
}

void Gui::setUpdateType(Gui::UpdateType type)
{
    m_updateType = type;
}

int Gui::updateTime() const
{
    return m_updateTime;
}

bool Gui::debug() const
{
    return m_updateDebug;
}

void Gui::setupImages()
{
    QStringList couleurs = QStringList() << "bleuC" << "bleuF" << "gris"
                                         << "jaune" << "rouge" << "vert" << "violet";
    for (int j=0; j<Constants::NumSpecies; ++j)
    {
        for (int i=0; i<couleurs.size(); ++i)
        {
            QPair<Alien::Color, Alien::Species> pair(static_cast<Alien::Color>(i), static_cast<Alien::Species>(j));
            QString path = QString(":/sprites/alien%1/%2.png").arg(j+1).arg(couleurs[i]);
            m_alienSprites[pair] = QIcon(path);
        }
    }
    m_foodSprite = QIcon(QString(":/sprites/pizza.png"));
}

void Gui::playOrPause()
{
    if (m_play->isChecked())
    {
        m_play->setText(trUtf8("Pause"));
        m_updateType = ALL;
        m_tick->setEnabled(false);
        m_turn->setEnabled(false);
    } else
    {
        m_play->setText(trUtf8("Continuer"));
        m_updateType = NONE;
        m_tick->setEnabled(true);
        m_turn->setEnabled(true);
    }
}

void Gui::tick()
{
    m_updateType = TICK;
}

void Gui::turn()
{
    m_updateType = TURN;
}

void Gui::setSpeed(int value)
{
    m_updateTime = 1000-value;
}

void Gui::setDebug(bool val)
{
    m_updateDebug = val;
    m_debug->setEnabled(val);
    m_debug->clear();
}

void Gui::appendDebug(const std::string& debug)
{
    m_debug->append(QString::fromStdString(debug));
}

void Gui::setStats(const std::string& stats)
{
    m_stats->setText(QString::fromStdString(stats));
}

void Gui::simulationOver(const std::string& msg)
{
    QMessageBox::information(this,
                             trUtf8("Simulation terminée"),
                             QString::fromStdString(msg));
}

void Gui::speciesDisqualified(const std::string& msg)
{
    QMessageBox::information(this,
                             trUtf8("Une espèce a été disqualifiée"),
                             QString::fromStdString(msg));
}
