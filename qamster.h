#ifndef qamster_H
#define qamster_H

#include <QMainWindow>
#include <memory>

#include "ui_qamster.h"
#include "timedatabase.h"
class QSqlRelationalTableModel;

class Qamster : public QMainWindow
{
Q_OBJECT
public:
    Qamster();
    virtual ~Qamster() {};

private:
  Ui::MainWindow ui_;
  TimeDatabase tdb_;
  std::unique_ptr<QSqlRelationalTableModel> rtm_;
  bool activity_running_ = false;
  QSqlRecord current_activity_;

  void stopActivity();
  void startActivity();

};

#endif // qamster_H
