#include <QApplication>
#include "qamster.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Qamster qamster;
    qamster.show();
    return app.exec();
}
