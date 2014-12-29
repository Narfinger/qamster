/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  Narfinger
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include "activityitemdelegate.h"
#include "helperfunctions.h"
#include "history.h"
#include "timedatabase.h"
#include "timetablemodel.h"

#include <functional>
#include <QDebug>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>
#include <QPair>
#include <QProgressBar>

History::History(QSqlDatabase db, QWidget* parent) : QDialog(parent), db_(db) { 
  ui_.setupUi(this);
  connect(ui_.buttonBox, &QDialogButtonBox::rejected, this, &History::reject);
  connect(ui_.tabWidget, &QTabWidget::currentChanged, [=]() { activated(ui_.calendarWidget->selectedDate()); });
  connect(ui_.calendarWidget, &QCalendarWidget::clicked, this, &History::activated);

  QSqlQuery q("SELECT start FROM time ORDER BY start LIMIT 1", db_);
  q.exec();
  q.next();
  const QDate min = q.value(0).toDateTime().date();
  q.exec("SELECT end FROM time ORDER BY start DESC LIMIT 1");
  q.next();
  const QDate max = q.value(0).toDateTime().date();
  
  ui_.calendarWidget->setMinimumDate(min);
  ui_.calendarWidget->setMaximumDate(max);
  
  ui_.w_acthistory->setDb(db_);
  ui_.w_cathistory->setDb(db_);

  //setup detailed day
  dd_ttm_ = QSharedPointer<TimeTableModel>(new TimeTableModel(this, db_));
  ui_.dd_tableView->setModel(dd_ttm_.data());
  dd_ttm_->select();
  ui_.dd_tableView->setColumnHidden(0, true);
  ui_.dd_tableView->setColumnHidden(6, true);
  ui_.dd_tableView->setItemDelegate(new ActivityItemDelegate);

  d_activated(QDate::currentDate());

  //restore from qsettings
  QSettings s("qamster");
  ui_.splitter->restoreState(s.value("history_splitter").toByteArray());
  restoreGeometry(s.value("history_dialog").toByteArray());
}

History::~History() {
  QSettings s("qamster");
  s.setValue("history_splitter", ui_.splitter->saveState());
  s.setValue("history_dialog", saveGeometry());
}

/** this is some c++ magic which i got from some guy on irc
 * notes: decltype(&foo) need the &
 * in C++14 we can omit the return type and write auto
 */
template <typename Func, typename Date>
typename std::result_of<Func(int)>::type History::getTotal(const Date& start, const Date& end, Func f) {
  QSqlQuery tquery = TDBHelper::queryTimeSubstitution("SELECT sum(strftime('%s', end)- strftime('%s', start)) from time where start>='%1' and start<='%2'",
						      db_, start, end);
  tquery.exec();
  tquery.next();
  const int value = tquery.value(0).toInt();
  return f(value);
}

const QPair<QDate, QDate> History::getWeek(const QDate& date) {
  const int day = date.dayOfWeek();
  const QDate start = date.addDays(-(day-1));
  const QDate end = date.addDays(7-day);
  return QPair<QDate, QDate>(start, end);
}

void History::activated(const QDate& date) {
  switch(ui_.tabWidget->currentIndex()) {
    case 0: d_activated(date); break;
    case 1: dd_activated(date); break;
    case 2: w_activated(date); break;
    case 3: g_activated(); break;
  }
}

void History::insertProgressBarIntoTable(QTableWidget* w, const QString& one, const QTime& time, const int totalsecs) {
  if (w==nullptr) { qDebug() << "null tablewidget given"; return; }
  const int row = w->rowCount();
  w->insertRow(row);
  QTableWidgetItem* c = new QTableWidgetItem(one);
  QProgressBar* b = new QProgressBar();
  b->setMaximum(totalsecs);
  const int seconds = time.hour()*60*60 + time.minute()*60 + time.second();
  b->setValue(seconds);
  b->setFormat(time.toString("h:mm"));
  w->setItem(row, 0, c);
  w->setCellWidget(row, 1, b);

  const double percentage = static_cast<double>(seconds) / static_cast<double>(totalsecs);
  QTableWidgetItem* p = new QTableWidgetItem(QString::number(percentage, 'f', 2));
  w->setItem(row, 2, p);
}

void History::insertItemIntoTable(QTableWidget* w, const QString& one, const QString& two) {
  if (w==nullptr) { qDebug() << "null tablewidget given"; return; }
  const int row = w->rowCount();
  w->insertRow(row);
  QTableWidgetItem *c = new QTableWidgetItem(one);
  QTableWidgetItem *d = new QTableWidgetItem(two);
  d->setTextAlignment(Qt::AlignCenter);
  w->setItem(row, 0, c);
  w->setItem(row, 1, d);
}

void History::d_fillCategory(const QDate& date) {
  ui_.d_categoryTable->setRowCount(0);

  const QDateTime start(date);
  const QDateTime end = QDateTime(date.addDays(1)).addSecs(-1);
  
  QString qstring("SELECT category.id AS cid, category.name,sum(strftime('%s', end) - strftime('%s', start)) AS diff FROM \
  time INNER JOIN category ON category.id = time.category WHERE start>='%1' and start<='%2' GROUP BY category ORDER BY diff DESC");
  qstring = qstring.arg(start.toString(TimeDatabase::DATEFORMAT)).arg(end.toString(TimeDatabase::DATEFORMAT));
  QSqlQuery cquery(qstring, db_);

  const QTime totaltime = getTotal(start, end);
  const int totalsecs = totaltime.hour()*60*60 + totaltime.minute()*60 + totaltime.second();

  cquery.exec();
  while (cquery.next()) {
    const QTime t = TDBHelper::secsToQTime(cquery.value(2).toInt());
    insertProgressBarIntoTable(ui_.d_categoryTable, cquery.value(1).toString(), t, totalsecs);
  }
  //total
  insertItemIntoTable(ui_.d_categoryTable, "Total", totaltime.toString("h:mm"));
}

void History::d_fillActivity(const QDate& date) {
  ui_.d_activityTable->setRowCount(0);

  const QDateTime start(date);
  const QDateTime end = QDateTime(date.addDays(1)).addSecs(-1);

  QString qstring("SELECT activity, sum(strftime('%s', end) - strftime('%s', start)) AS diff FROM \
  time WHERE start>='%1' and start<='%2' GROUP BY activity ORDER BY diff DESC");
  qstring = qstring.arg(start.toString(TimeDatabase::DATEFORMAT)).arg(end.toString(TimeDatabase::DATEFORMAT));
  QSqlQuery cquery(qstring, db_);

  const QTime totaltime = getTotal(start, end);
  const int totalsecs = totaltime.hour()*60*60 + totaltime.minute()*60 + totaltime.second();
  cquery.exec();
  while (cquery.next()) {
    const QTime t = TDBHelper::secsToQTime(cquery.value(1).toInt());
    insertProgressBarIntoTable(ui_.d_activityTable, cquery.value(0).toString(), t, totalsecs);
  }
  //total
  insertItemIntoTable(ui_.d_activityTable, "Total", totaltime.toString("h:mm"));
}

void History::d_activated(const QDate& date) {
  d_fillCategory(date);
  d_fillActivity(date);

  //number of context switches
  QSqlQuery q = TDBHelper::queryTimeSubstitution("SELECT count(*) FROM time WHERE start>='%1' and start<='%2'",
						 db_, date);
  q.exec();
  q.next();
  const int contextswitches = std::max(q.value(0).toInt() -1, 0);
  ui_.d_contextSwitch->setText(QString::number(contextswitches));
}

void History::dd_activated(const QDate& date) {
  const QDateTime start(date);
  const QDateTime end = QDateTime(date.addDays(1)).addSecs(-1);
  const QString filterstring = "start>='" + start.toString(TimeDatabase::DATEFORMAT) + "' AND end<='" + end.toString(TimeDatabase::DATEFORMAT) + "'";
  dd_ttm_->setFilter(filterstring);
  dd_ttm_->select();
}

void History::w_activated(const QDate& date) {
  const QPair<QDate,QDate> pdate = getWeek(date);
  const QDate start = pdate.first;
  const QDate end = pdate.second;
  ui_.w_acthistory->drawWeek(start, end);
  ui_.w_cathistory->drawWeek(start, end);
  
  const QString total = getTotal(start, end, std::function<QString(int)>(TDBHelper::secsToQString));
  ui_.w_total->setText(total);
}

void History::g_activated() {
  const QString total = getTotal(QDate(1,0,0), QDate::currentDate(), std::function<QString(int)>(TDBHelper::secsToQString));
  ui_.g_totalTrack->setText(total);

  const QDate start(1,0,0);
  const QDate end = QDate::currentDate();

  QSqlQuery q = TDBHelper::queryTimeSubstitution("SELECT activity, sum(strftime('%s', end) - strftime('%s', start)) AS diff FROM \
						 time WHERE start>='%1' and start<='%2' GROUP BY activity ORDER BY diff DESC",
						 db_, start, end);
  q.exec();
  while (q.next()) {
    const QString t = TDBHelper::secsToQString(q.value(1).toInt());
    insertItemIntoTable(ui_.g_categoryTable, q.value(0).toString(), t);
  }
}

