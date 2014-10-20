#include "qamster.h"

#include "activityitemdelegate.h"
#include "timedatabase.h"
#include "timetablemodel.h"
#include "timetableview.h"



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
  connect(ui_.tableView, &TimeTableView::start, this, &Qamster::doubleClicked);
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

void Qamster::doubleClicked(const QModelIndex& index) {
  QStringList slist;
  const QModelIndex nameindex = rtm_->index(index.row(), TimeDatabase::T_ACTIVITY);
  slist << rtm_->data(nameindex).toString();
  const QModelIndex categoryindex = rtm_->index(index.row(), TimeDatabase::T_CATEGORY);
  slist << rtm_->data(categoryindex).toString();
  startActivityStrings(slist);
}

void Qamster::stopActivity() {
  rtm_->stopActivity();
  ui_.checkBox->setCheckState(Qt::Unchecked);
  ui_.activityLabel->clear();
}

void Qamster::startActivity() {
  const QStringList slist = ui_.activityEdit->text().split("@");
  startActivityStrings(slist);
}

void Qamster::startActivityStrings(const QStringList& slist) {
  if (slist.size()==1) {
    rtm_->startActivity(slist[0], "uncategorized");
  } else {
    rtm_->startActivity(slist[0], slist[1]);
  }
  ui_.activityEdit->clear();
  ui_.activityLabel->setText(slist[0]);
  ui_.checkBox->setCheckState(Qt::Checked);
}

