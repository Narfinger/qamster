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

  connect(ui_.activityEdit, &QLineEdit::returnPressed, this, &Qamster::startActivity);
  connect(ui_.stopActivityButton, &QPushButton::pressed, this, &Qamster::stopActivity);
  connect(rtm_.get(), &TimeTableModel::minutesPassed, this, &Qamster::minutesPassed);
}

void Qamster::minutesPassed(const int minutes) {
  const div_t divresult = div(minutes, 60);
  QString text;
  if (divresult.quot > 0) {
    text = QString("%1 h %2 min").arg(divresult.quot).arg(divresult.rem);
  } else {
    text = QString("%1 min").arg(minutes);
  }
  ui_.timeLabel->setText(text);
}

void Qamster::stopActivity() {
  rtm_->stopActivity();
  ui_.checkBox->setCheckState(Qt::Unchecked);
}

void Qamster::startActivity() {
  rtm_->startActivity(ui_.activityEdit->text(), "1");
  ui_.activityEdit->clear();
  ui_.checkBox->setCheckState(Qt::Checked);
}
