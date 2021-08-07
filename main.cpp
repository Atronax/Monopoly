#include <QApplication>
#include <QTime>

#include "table.h"

int main (int argc, char* argv[])
{
    srand(QTime::currentTime().msec());

    QApplication app (argc, argv);    

    Table ui;
    ui.show();

    return app.exec();
}
