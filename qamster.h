#ifndef qamster_H
#define qamster_H

#include <QMainWindow>
#include <memory>

#include "ui_qamster.h"
#include "timedatabase.h"
#include "timetablemodel.h"

class Qamster : public QMainWindow
{
Q_OBJECT
public:
    Qamster();
    virtual ~Qamster() {};

private:
  Ui::MainWindow ui_;
  TimeDatabase tdb_;
  std::unique_ptr<TimeTableModel> rtm_;

  void minutesPassed(const int minutes);

  void stopActivity();
  void startActivity();

};

#endif // qamster_H
