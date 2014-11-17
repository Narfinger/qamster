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

void History::d_fillCategory(const QDate& date) {
  ui_.d_activityTable->setRowCount(0);
  ui_.d_categoryTable->setRowCount(0);

  const QDateTime start(date);
  const QDateTime end = QDateTime(date.addDays(1)).addSecs(-1);

  //total sum
  QString tqstring("SELECT sum(strftime('%s', end)- strftime('%s', start)) from time where start>='%1' and start<='%2'");
  tqstring = tqstring.arg(start.toString(TimeDatabase::DATEFORMAT)).arg(end.toString(TimeDatabase::DATEFORMAT));
  QSqlQuery tquery(tqstring, db_);
  tquery.exec();
  tquery.next();
  const QTime totaltime = TDBHelper::secsToQTime(tquery.value(0).toInt());

  QString qstring("SELECT category.id AS cid, category.name,sum(strftime('%s', end) - strftime('%s', start)) AS diff FROM \
  time INNER JOIN category ON category.id = time.category WHERE start>='%1' and start<='%2' GROUP BY category ORDER BY diff");
  qstring = qstring.arg(start.toString(TimeDatabase::DATEFORMAT)).arg(end.toString(TimeDatabase::DATEFORMAT));
  QSqlQuery cquery(qstring, db_);
 /* QSqlQuery cquery(db_);
  cquery.prepare("SELECT category.id AS cid, category.name,sum(strftime('%s', end) - strftime('%s', start)) AS diff FROM \
  time INNER JOIN category ON category.id = time.category WHERE start>=:start and start<=:end GROUP BY category ORDER BY diff");
  cquery.bindValue(":start", "'" + start.toString(TimeDatabase::DATEFORMAT) + "'");
  cquery.bindValue(":end", "'" + end.toString(TimeDatabase::DATEFORMAT) + "'");*/

  cquery.exec();
  int row = 0;
  while (cquery.next()) {
    ui_.d_categoryTable->insertRow(row);
    QTableWidgetItem *c = new QTableWidgetItem(cquery.value(1).toString());
    const QTime t = TDBHelper::secsToQTime(cquery.value(2).toInt());
    QTableWidgetItem *d = new QTableWidgetItem(t.toString("h:mm"));
    ui_.d_categoryTable->setItem(row, 0, c);
    ui_.d_categoryTable->setItem(row, 1, d);

    row++;
  }

  //total
  ui_.d_categoryTable->insertRow(row);
  QTableWidgetItem *c = new QTableWidgetItem("Total");
  QTableWidgetItem *d = new QTableWidgetItem(totaltime.toString("h:mm"));
  ui_.d_categoryTable->setItem(row, 0, c);
  ui_.d_categoryTable->setItem(row, 1, d);
}

void History::d_activated(const QDate& date) {
  d_fillCategory(date);
}
