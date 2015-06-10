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
#include <QStringList>
#include <QTime>

#include "helperfunctions.h"
#include "timedatabase.h"
#include "timetablemodel.h"
#include "settings.h"

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
  setEditStrategy(QSqlTableModel::OnManualSubmit);
  setSort(TimeDatabase::T_START, Qt::AscendingOrder);

  current_activity_ = record();
  //set filter
  update(true);

  connect(&timer_, &QTimer::timeout, this, [=](){ minutes_passed_++; emit minutesPassed(minutes_passed_); });
}

TimeTableModel::~TimeTableModel() {
  if(activity_running_)
    stopActivity();
}

int TimeTableModel::rowCount(const QModelIndex& parent) const {
  const int count = QSqlTableModel::rowCount(parent);
  if (activity_running_) return count+1;
  return count;
}

QVariant TimeTableModel::data(const QModelIndex& item, int role) const
{
  if (role == Qt::BackgroundColorRole) return backgroundColorRoleData(item, role);
  if (role == Qt::TextColorRole) return textColorRoleData(item, role);

  if (activity_running_ && QSqlRelationalTableModel::rowCount() == item.row()) return runningActivityData(item,role);

  if (item.column() == 5) {
    if (role==Qt::DisplayRole) {
      const QString startstring = data(item.sibling(item.row(), TimeDatabase::T_START)).toString();
      const QString endstring = data(item.sibling(item.row(), TimeDatabase::T_END)).toString();
      const QDateTime start = QDateTime::fromString(startstring, TimeDatabase::DATEFORMAT);
      const QDateTime end = QDateTime::fromString(endstring, TimeDatabase::DATEFORMAT);
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

bool TimeTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (activity_running_ && index.row() == rowCount() -1) {
    switch (index.column()) {
      case 3: current_activity_.setValue(TimeDatabase::T_ACTIVITY, value); return true;
      case 4: return false; //! TODO yet to implement
    }
    return false;
  }

  return QSqlRelationalTableModel::setData(index, value, role);
}

bool TimeTableModel::setData(const QModelIndex& index, const QDateTime& d, int role) {
  if (index.column() != 1 && index.column() != 2) return false;
  if (activity_running_ && index.row() == rowCount() -1) {
    if (index.column() == 1) {
      current_activity_.setValue(TimeDatabase::T_START, d.toString(TimeDatabase::DATEFORMAT));
      return true;
    } else
      return false;
  }
  const QString date = d.toString(TimeDatabase::DATEFORMAT);
  return setData(index, date, role);
}

Qt::ItemFlags TimeTableModel::flags(const QModelIndex& index) const {
  if (index.column()== 5 || index.column()==6)
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  else
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

void TimeTableModel::stopActivity() {
  const QDateTime d(QDateTime::currentDateTime());
  if (activity_running_) {
    current_activity_.setValue(TimeDatabase::T_END, d.toString(TimeDatabase::DATEFORMAT));
    insertRowIntoTable(current_activity_);
    current_activity_.clearValues();
    activity_running_ = false;
    timer_.stop();
    update();
  }
}

void TimeTableModel::startActivity(const QString& name, const QString& category) {
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

  //check if we need to stop or this just appends
  if (activity_running_) {
    const bool sameactivity = current_activity_.value(TimeDatabase::T_ACTIVITY).toString() == name 
    && (current_activity_.value(TimeDatabase::T_CATEGORY) == cid);
    if (sameactivity)
      return;
    stopActivity();
  }

  //prepare QSqlRecord
  const QDateTime d(QDateTime::currentDateTime());
  current_activity_.setValue(TimeDatabase::T_START, d.toString(TimeDatabase::DATEFORMAT));
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
	       FROM time INNER JOIN  category ON category.id = time.category WHERE date(end)>=:date GROUP BY cid ORDER BY cid");
  sums.bindValue(":date", d.toString(TimeDatabase::DATEFORMAT));
  sums.exec();
  total.prepare("SELECT sum(strftime('%s', end) - strftime('%s', start)) AS sum \
		FROM time WHERE date(end)>=:date");
  total.bindValue(":date", d.toString(TimeDatabase::DATEFORMAT));
  total.exec();
  total.next();

  const unsigned long long totalsecs = total.value(0).toLongLong();
  QString result;
  while (sums.next()) {
    const QString cname = sums.value(1).toString();
    const unsigned long long cattotal = sums.value(2).toLongLong();
    const double ratio = static_cast<double>(cattotal) / static_cast<double>(totalsecs);
    const QTime t = TDBHelper::secsToQTime(cattotal);

    result.append(QString("%1: %2h (%3%) | ").arg(cname).arg(t.toString("H:mm")).arg(QString::number(ratio, 'f', 2)));
  }
  const QTime t = TDBHelper::secsToQTime(totalsecs);
  result.append(QString("total: %1h").arg(t.toString("H:mm")));
  return result;
}

void TimeTableModel::update(const bool force) {
  if(force || currentdate_ != QDate::currentDate()) {
    //set filter for today
    const QDateTime d(QDate::currentDate().addDays(-1));
    setFilter("date(end)>='" + d.toString(TimeDatabase::DATEFORMAT) + "'");
    currentdate_ = d.date();
  }
  select();
}

const QStringList TimeTableModel::categories() const {
  QSqlQuery q(database());
  q.exec("SELECT name FROM category ORDER BY id");
  QStringList l;
  while (q.next()) {
    l << q.value(0).toString();
  }
  return l;
}

int TimeTableModel::categoryIdForIndex(const QModelIndex& index) const {
  const int id = data(index.sibling(index.row(), 0)).toInt();
  QSqlQuery q(database());
  q.prepare("SELECT category FROM time WHERE id = :id");
  q.bindValue(":id", id);
  q.exec();
  if (!q.next()) return 0;
  return q.value(0).toInt() -1; //indices start at one cbox starts at zero
}

void TimeTableModel::setCategory(const QModelIndex& index, const int c) {
  const int id = data(index.sibling(index.row(), 0)).toInt();
  QSqlQuery q(database());
  q.prepare("UPDATE time SET category = :cid WHERE id = :id");
  q.bindValue(":cid", c +1); //indices start at one cbox starts at zero
  q.bindValue(":id", id);
  q.exec();
}

void TimeTableModel::deleteCurrentActivity(const QModelIndex& index) {
  this->removeRow(index.row());
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

QVariant TimeTableModel::backgroundColorRoleData(const QModelIndex& index, int role) const {
  QSettings s("qamster");
  if (s.value(COLOR_CAT_IN_MAIN).toBool()) {
    const int id = categoryIdForIndex(index);
    QSqlQuery q(database());
    q.prepare("SELECT color FROM category WHERE id = :id");
    q.bindValue(":id", id+1);
    q.exec();
    q.next();
    return TDBHelper::stringToColor(q.value(0));
  } else {
    return QSqlRelationalTableModel::data(index, role);
  }
}

QVariant TimeTableModel::textColorRoleData(const QModelIndex& index, int role) const {
  QSettings s("qamster");
  if (s.value(COLOR_CAT_IN_MAIN).toBool()) {
    return s.value(TEXT_COLOR);
  } else {
    return QSqlRelationalTableModel::data(index, role);
  }
}
