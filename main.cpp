#include <QtGui/QApplication>
#include "qamster.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    qamster qamster;
    qamster.show();
    return app.exec();
}
