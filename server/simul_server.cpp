#include <QtWidgets>
#include <iostream>
#include "../src/game.h"
#include "server.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    QFontDatabase fontDB;
    int fontId = fontDB.addApplicationFont(":/sprites/font/Symbola.ttf");
    QStringList fonts = fontDB.applicationFontFamilies(fontId);
    if (fonts.empty())
    {
        cerr << "La police n'a pas été trouvée ..." << endl;
        return 1;
    }

    Server* s;
    string host("127.0.0.1");
    int port  = 9912;
    bool local = false;
    for (int i=1; i<argc; ++i)
    {
        string arg(argv[i]);
        if(arg == "-l")
            local = true;
        //TODO : ajoutez le support pour les arguments du port et l'host
    }

    s = new TcpServer(host, port);
    s->start();

    //attend des clients
    if (!local)
        s->waitUntilReady();

    Game game(s, 50, 40, 25, local);

    while (!game.simulationOver())
        game.update();

    s->stop();

    delete s;
    fontDB.removeAllApplicationFonts();

    return 0;
}

