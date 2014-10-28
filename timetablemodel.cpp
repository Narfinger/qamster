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

#include <QDebug>
#include <QSqlError>
#include <QTime>

#include "timedatabase.h"
#include "timetablemodel.h"

const QString TimeTableModel::DATEFORMAT = "yyyy-MM-dd HH:mm:ss";

TimeTableModel::TimeTableModel(QObject* parent, QSqlDatabase db) :
    QSqlRelationalTableModel(parent, db), currentdate_(QDate::currentDate()) {
  setTable("time");
  setHeaderData(1, Qt::Horizontal, "Start Time");
  setHeaderData(2, Qt::Horizontal, "End Time");
  setHeaderData(3, Qt::Horizontal, "Activity");
  setHeaderData(4, Qt::Horizontal, "Category");
  setHeaderData(5, Qt::Horizontal, "Time");
  setHeaderData(6, Qt::Horizontal, "");

  setRelation(4, QSqlRelation("category", "id", "name"));
  setEditStrategy(QSqlTableModel::OnFieldChange);
  setSort(TimeDatabase::T_START, Qt::AscendingOrder);

  current_activity_ = record();
  //set filter
  update(true);

  connect(&timer_, &QTimer::timeout, this, [=](){ minutes_passed_++; emit minutesPassed(minutes_passed_); });
}

int TimeTableModel::rowCount(const QModelIndex& parent) const {
  const int count = QSqlTableModel::rowCount(parent);
  if (activity_running_) return count+1;
  return count;
}

QVariant TimeTableModel::data(const QModelIndex& item, int role) const
{
  if (activity_running_ && QSqlRelationalTableModel::rowCount() == item.row()) return runningActivityData(item,role);

  if (item.column() == 5) {
    if (role==Qt::DisplayRole) {
      const QString startstring = data(item.sibling(item.row(), TimeDatabase::T_START)).toString();
      const QString endstring = data(item.sibling(item.row(), TimeDatabase::T_END)).toString();
      const QDateTime start = QDateTime::fromString(startstring, TimeTableModel::DATEFORMAT);
      const QDateTime end = QDateTime::fromString(endstring, TimeTableModel::DATEFORMAT);
      const QTime diff = QTime::fromMSecsSinceStartOfDay(end.toMSecsSinceEpoch() - start.toMSecsSinceEpoch());
      
      QString format = QString("%1min").arg(diff.minute());
      if (diff.hour()!=0)
	format.prepend(QString("%1h ").arg(diff.hour()));
      return QVariant(format);
    } else {
      const QModelIndex i = item.sibling(item.row(), item.column()-1); //item.parent().child(item.row(), item.column()-1);
      return QSqlRelationalTableModel::data(i, role);
    }
  } else if (item.column() == 6) {
    if (role == Qt::DecorationRole) {
      return QIcon::fromTheme("document-properties");
    } else return QVariant();
  } else
      return QSqlRelationalTableModel::data(item, role);
}

Qt::ItemFlags TimeTableModel::flags(const QModelIndex& index) const {
  Q_UNUSED(index);
  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void TimeTableModel::stopActivity() {
  const QDateTime d(QDateTime::currentDateTime());
  if (activity_running_) {
    current_activity_.setValue(TimeDatabase::T_END, d.toString(TimeTableModel::DATEFORMAT));
    insertRowIntoTable(current_activity_);
    QSqlError e = lastError();
    //qDebug() << e << e.type() << e.databaseText() << e.driverText();
    current_activity_.clearValues();
    activity_running_ = false;
    timer_.stop();
    update();
  }
}

void TimeTableModel::startActivity(const QString& name, const QString& category) {
  if (activity_running_) {
    stopActivity();
  }
  //check if we need to insert category and insert it
  QSqlQuery q(database());
  q.prepare("SELECT count(*) FROM category WHERE name = :name");
  q.bindValue(":name", category);
  q.exec();
  q.next();
  const int size = q.value(0).toInt();

  //insert new category
  QSqlTableModel* catmodel = relationModel(TimeDatabase::T_CATEGORY);
  QSqlRecord r(catmodel->record());
  if (size == 0) {
    r.setValue(1, category);
    catmodel->insertRecord(-1, r);
    catmodel->select();	//this updates the internal model otherwise new category is not yet there
  }

  //get id
  QSqlQuery idq(database());
  idq.prepare("SELECT id AS 'id' FROM category WHERE name = :name");
  idq.bindValue(":name", category);
  idq.exec();
  idq.next();
  const int cid = idq.value(0).toInt();

  //prepare QSqlRecord
  const QDateTime d(QDateTime::currentDateTime());
  current_activity_.setValue(TimeDatabase::T_START, d.toString(TimeTableModel::DATEFORMAT));
  current_activity_.setValue(TimeDatabase::T_ACTIVITY, name);
  current_activity_.setValue(TimeDatabase::T_CATEGORY, cid);
  activity_running_ = true;

  timer_.start(60*1000);
  minutes_passed_ = 0;

  update();
}

const QString TimeTableModel::getTodaysStatusbarText() const {
  const QDateTime d(QDate::currentDate().addDays(-1));
  QSqlQuery sums(database());
  QSqlQuery total(database());
  //gives in secs
  sums.prepare("SELECT category.id AS cid,category.name AS name,sum(strftime('%s', end) - strftime('%s', start)) AS diff \
	       FROM time INNER JOIN  category ON category.id = time.category GROUP BY cid ORDER BY cid");
  sums.bindValue(":date", d.toString(TimeTableModel::DATEFORMAT));
  sums.exec();
  total.prepare("SELECT sum(strftime('%s', end) - strftime('%s', start)) AS sum \
		FROM time WHERE date(end)>=:date");
  total.bindValue(":date", d.toString(TimeTableModel::DATEFORMAT));
  total.exec();
  total.next();

  const unsigned long long totalsecs = total.value(0).toLongLong();
  QString result;
  while (sums.next()) {
    const QString cname = sums.value(1).toString();
    const unsigned long long cattotal = sums.value(2).toLongLong();
    const double ratio = static_cast<double>(cattotal) / static_cast<double>(totalsecs);
    const QTime t(0,0,cattotal);

    result.append(QString("%1: %2h (%3%) | ").arg(cname).arg(t.toString("H:mm")).arg(QString::number(ratio, 'f', 2)));
  }
  const QTime t(0,0,totalsecs);
  result.append(QString("total: %1").arg(t.toString("H:mm")));
  return result;
}

void TimeTableModel::update(const bool force) {
  if(force || currentdate_ != QDate::currentDate()) {
    //set filter for today
    const QDateTime d(QDate::currentDate().addDays(-1));
    setFilter("date(end)>='" + d.toString(TimeTableModel::DATEFORMAT) + "'");
    currentdate_ = d.date();
  }
  select();
}

QVariant TimeTableModel::runningActivityData(const QModelIndex& item, int role) const {
  if (role == Qt::DecorationRole && item.column() == 6) return QIcon::fromTheme("document-properties");
  if (role != Qt::DisplayRole) return QSqlRelationalTableModel::data(item, role);
  if (role == Qt::DisplayRole && item.column() == 4) {	//get the correct category name
    QSqlQuery q(database());
    q.prepare("SELECT name FROM category WHERE id = :id");
    q.bindValue(":id", current_activity_.value(4).toInt());
    q.exec();
    q.next();
    return q.value(0);
  }

  return current_activity_.value(item.column());
}
