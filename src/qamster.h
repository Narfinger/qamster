#ifndef qamster_H
#define qamster_H

#include <QMainWindow>
#include <memory>

#include "ui_qamster.h"
#include "activitycompletermodel.h"
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
  QSharedPointer<TimeTableModel> rtm_;
  std::unique_ptr<ActivityCategoryCompleterModel> acm_;
  QLabel* sbarText;

  void minutesPassed(const int minutes);
  void stateChanged();	//call whenever state changed to update some stuff

  void doubleClicked(const QModelIndex& index);
  void stopActivity();
  void startActivityFromLineEdit();
  void startActivityStrings(const QStringList& slist);

private slots:
  void showHistory();
};

#endif // qamster_H
