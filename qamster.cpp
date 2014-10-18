#include "qamster.h"

#include "activityitemdelegate.h"
#include "timetablemodel.h"



#include <QDateTime>


Qamster::Qamster() {
  ui_.setupUi(this);
  { //i don't know a different way. scoped because of tm would leak outside
    std::unique_ptr<TimeTableModel> tm(new TimeTableModel(this, tdb_.connect()));
    rtm_ = std::move(tm);
  }

  //rtm_->setRelation(3, QSqlRelation("category", "id", "name"));
  ui_.tableView->setModel(rtm_.get());
  ui_.tableView->setColumnHidden(0, true);
  ui_.tableView->setItemDelegate(new ActivityItemDelegate);

  QSqlDatabase db(tdb_.connect());
  QSqlQuery q(db);
  q.exec("select * from time;");
  while (q.next()) {
    qDebug() << q.value(0) << q.value(1);
  }

  current_activity_ = rtm_->database().record("time");

  connect(ui_.activityEdit, &QLineEdit::returnPressed, this, &Qamster::startActivity);
  connect(ui_.stopActivityButton, &QPushButton::pressed, this, &Qamster::stopActivity);
}

void Qamster::stopActivity() {
  const QDateTime d(QDateTime::currentDateTime());
  if (activity_running_) {
    current_activity_.setValue(TimeDatabase::T_END, d.toString(Qt::ISODate));
    rtm_->insertRecord(-1, current_activity_);

    qDebug() << "record inserted:" << current_activity_;
    current_activity_.clearValues();
    ui_.checkBox->setCheckState(Qt::Unchecked);
    activity_running_ = false;
    rtm_->select();
  }
}

void Qamster::startActivity() {
  if (activity_running_) {
    stopActivity();
  }
  const QDateTime d(QDateTime::currentDateTime());

  current_activity_.setValue(TimeDatabase::T_START, d.toString(Qt::ISODate));
  current_activity_.setValue(TimeDatabase::T_ACTIVITY, ui_.activityEdit->text());
  current_activity_.setValue(TimeDatabase::T_CATEGORY, 1);
  ui_.activityEdit->clear();

  activity_running_ = true;
  ui_.checkBox->setCheckState(Qt::Checked);
}
