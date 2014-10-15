#ifndef qamster_H
#define qamster_H

#include <QMainWindow>
#include "ui_qamster.h"


class Qamster : public QMainWindow
{
Q_OBJECT
private:
  Ui::MainWindow ui_;
public:
    Qamster();
    virtual ~Qamster() {};
};

#endif // qamster_H
