#include "qamster.h"

#include "activityitemdelegate.h"
#include "editdialog.h"
#include "history.h"
#include "timedatabase.h"
#include "timetablemodel.h"
#include "timetableview.h"

#include <QCompleter>
#include <QDateTime>
#include <QSqlQuery>

Qamster::Qamster() {
  ui_.setupUi(this);
  { //i don't know a different way. scoped because of tm would leak outside
    rtm_ = QSharedPointer<TimeTableModel>(new TimeTableModel(this, tdb_.connect()));
    /*std::unique_ptr<TimeTableModel> tm(new TimeTableModel(this, tdb_.connect()));
    rtm_ = std::move(tm);*/
  }
  {
    std::unique_ptr<ActivityCompleterModel> up(new ActivityCompleterModel(tdb_.connect(), this));
    acm_ = std::move(up);
  }

  //rtm_->setRelation(3, QSqlRelation("category", "id", "name"));
  ui_.tableView->setModel(rtm_.data());
  ui_.tableView->setColumnHidden(0, true);
  ui_.tableView->setItemDelegate(new ActivityItemDelegate);

  sbarText = new QLabel(this);
  statusBar()->addPermanentWidget(sbarText);

  QCompleter* c = new QCompleter(acm_.get(), ui_.activityEdit);
  c->setCaseSensitivity(Qt::CaseInsensitive);
  c->setCompletionMode(QCompleter::PopupCompletion);
  ui_.activityEdit->setCompleter(c);

  connect(ui_.activityEdit, &QLineEdit::returnPressed, this, &Qamster::startActivityFromLineEdit);
  connect(c, SIGNAL(activated(const QString&)), ui_.activityEdit, SLOT(clear()), Qt::QueuedConnection);	//i could not get this to work with the new syntax
  connect(ui_.stopActivityButton, &QPushButton::pressed, this, &Qamster::stopActivity);
  connect(ui_.refilterButton, &QPushButton::pressed, [=]() { rtm_->update();
							     stateChanged(); });
  connect(rtm_.data(), &TimeTableModel::minutesPassed, this, &Qamster::minutesPassed);
  connect(ui_.tableView, &TimeTableView::start, this, &Qamster::doubleClicked);

  //actions
  connect(ui_.actionHistory, &QAction::triggered, this, &Qamster::showHistory);
  connect(ui_.actionQuit, &QAction::triggered, qApp, &QApplication::quit);

  stateChanged();
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

void Qamster::stateChanged() {
  sbarText->setText(rtm_->getTodaysStatusbarText());
  acm_->select();
}

void Qamster::doubleClicked(const QModelIndex& index) {
  if (index.column() == 6) {
    EditDialog d(rtm_, tdb_.connect(), index);
    d.exec();
    rtm_->update();
  } else {
    QStringList slist;
    const QModelIndex nameindex = rtm_->index(index.row(), TimeDatabase::T_ACTIVITY);
    slist << rtm_->data(nameindex).toString();
    const QModelIndex categoryindex = rtm_->index(index.row(), TimeDatabase::T_CATEGORY);
    slist << rtm_->data(categoryindex).toString();
    startActivityStrings(slist);
  }
}

void Qamster::stopActivity() {
  rtm_->stopActivity();
  ui_.checkBox->setCheckState(Qt::Unchecked);
  ui_.activityLabel->clear();
  ui_.timeLabel->setText("0 min");

  stateChanged();
}

void Qamster::startActivityFromLineEdit() {
  stopActivity();
  
  const QStringList slist = ui_.activityEdit->text().split("@");
  startActivityStrings(slist);

  ui_.activityEdit->clear();
}

void Qamster::startActivityStrings(const QStringList& slist) {
  stopActivity();
  if (slist.size()==1) {
    rtm_->startActivity(slist[0], "uncategorized");
  } else {
    rtm_->startActivity(slist[0], slist[1]);
  }
  stateChanged();

  ui_.activityLabel->setText(slist[0]);
  ui_.checkBox->setCheckState(Qt::Checked);
  ui_.tableView->scrollToBottom();
}

void Qamster::showHistory() {
  History h(tdb_.connect());
  h.exec();
}