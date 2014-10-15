#ifndef qamster_H
#define qamster_H

#include <QMainWindow>

#include "ui_qamster.h"
#include "timedatabase.h"

class Qamster : public QMainWindow
{
Q_OBJECT
public:
    Qamster();
    virtual ~Qamster() {};

private:
  Ui::MainWindow ui_;
  TimeDatabase tdb_;
};

#endif // qamster_H
