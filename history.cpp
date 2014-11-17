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

#include "helperfunctions.h"
#include "history.h"
#include "timedatabase.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

History::History(QSqlDatabase db, QWidget* parent) : QDialog(parent), db_(db) { 
  ui_.setupUi(this);
  connect(ui_.buttonBox, &QDialogButtonBox::rejected, this, &History::reject);
  connect(ui_.d_calendarWidget, &QCalendarWidget::activated, this, &History::d_activated);

  QSqlQuery q("SELECT start FROM time ORDER BY id LIMIT 1", db_);
  q.exec();
  q.next();
  const QDate min = q.value(0).toDateTime().date();
  q.exec("SELECT end FROM time ORDER BY id DESC LIMIT 1");
  q.next();
  const QDate max = q.value(0).toDateTime().date();
  
  ui_.d_calendarWidget->setMinimumDate(min);
  ui_.d_calendarWidget->setMaximumDate(max);

  d_activated(QDate::currentDate());
}

const QTime History::getTotal(const QDateTime& start, const QDateTime& end) {
  QString tqstring("SELECT sum(strftime('%s', end)- strftime('%s', start)) from time where start>='%1' and start<='%2'");
  tqstring = tqstring.arg(start.toString(TimeDatabase::DATEFORMAT)).arg(end.toString(TimeDatabase::DATEFORMAT));
  QSqlQuery tquery(tqstring, db_);
  tquery.exec();
  tquery.next();
  return TDBHelper::secsToQTime(tquery.value(0).toInt());
}

void History::insertIntoTable(QTableWidget* w, const QString& one, const QString& two) {
  const int row = w->rowCount();
  w->insertRow(row);
  QTableWidgetItem *c = new QTableWidgetItem(one);
  QTableWidgetItem *d = new QTableWidgetItem(two);
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
 /* QSqlQuery cquery(db_);
  cquery.prepare("SELECT category.id AS cid, category.name,sum(strftime('%s', end) - strftime('%s', start)) AS diff FROM \
  time INNER JOIN category ON category.id = time.category WHERE start>=:start and start<=:end GROUP BY category ORDER BY diff");
  cquery.bindValue(":start", "'" + start.toString(TimeDatabase::DATEFORMAT) + "'");
  cquery.bindValue(":end", "'" + end.toString(TimeDatabase::DATEFORMAT) + "'");*/

  cquery.exec();
  while (cquery.next()) {
    const QTime t = TDBHelper::secsToQTime(cquery.value(2).toInt());
    insertIntoTable(ui_.d_categoryTable, cquery.value(1).toString(), t.toString("h:mm"));
  }

  //total
  const QTime totaltime = getTotal(start, end);
  insertIntoTable(ui_.d_categoryTable, "Total", totaltime.toString("h:mm"));
}

void History::d_fillActivity(const QDate& date) {
  ui_.d_activityTable->setRowCount(0);

  const QDateTime start(date);
  const QDateTime end = QDateTime(date.addDays(1)).addSecs(-1);

  QString qstring("SELECT activity, sum(strftime('%s', end) - strftime('%s', start)) AS diff FROM \
  time WHERE start>='%1' and start<='%2' GROUP BY activity ORDER BY diff DESC");
  qstring = qstring.arg(start.toString(TimeDatabase::DATEFORMAT)).arg(end.toString(TimeDatabase::DATEFORMAT));
  QSqlQuery cquery(qstring, db_);

  cquery.exec();
  while (cquery.next()) {
    const QTime t = TDBHelper::secsToQTime(cquery.value(1).toInt());
    insertIntoTable(ui_.d_activityTable, cquery.value(0).toString(), t.toString("h:mm"));
  }

  //total
  const QTime totaltime = getTotal(start, end);
  insertIntoTable(ui_.d_activityTable, "Total", totaltime.toString("h:mm"));
}

void History::d_activated(const QDate& date) {
  d_fillCategory(date);
  d_fillActivity(date);
}
