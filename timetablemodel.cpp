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

#include <QTime>

#include "timedatabase.h"
#include "timetablemodel.h"

TimeTableModel::TimeTableModel(QObject* parent, QSqlDatabase db) : QSqlRelationalTableModel(parent, db) {
  setTable("time");
  setHeaderData(1, Qt::Horizontal, "Start Time");
  setHeaderData(2, Qt::Horizontal, "End Time");
  setHeaderData(3, Qt::Horizontal, "Activity");
  setHeaderData(4, Qt::Horizontal, "Category");
  setHeaderData(5, Qt::Horizontal, "Time");
  setEditStrategy(QSqlTableModel::OnRowChange);
  select();

  current_activity_ = database().record("time");

  connect(&timer_, &QTimer::timeout, this, [=](){ minutes_passed_++; emit minutesPassed(minutes_passed_); });
}

QVariant TimeTableModel::data(const QModelIndex& item, int role) const
{
  if (item.column()==5) {
    if (role==Qt::DisplayRole) {
      const QString startstring = data(item.sibling(item.row(), TimeDatabase::T_START)).toString();
      const QString endstring = data(item.sibling(item.row(), TimeDatabase::T_END)).toString();
      const QDateTime start = QDateTime::fromString(startstring, Qt::ISODate);
      const QDateTime end = QDateTime::fromString(endstring, Qt::ISODate);
      const QTime diff = QTime::fromMSecsSinceStartOfDay(end.toMSecsSinceEpoch() - start.toMSecsSinceEpoch());
      
      QString format = QString("%1min").arg(diff.minute());
      if (diff.hour()!=0)
	format.prepend(QString("%1h ").arg(diff.hour()));
      return QVariant(format);
    } else {
      const QModelIndex i = item.sibling(item.row(), item.column()-1); //item.parent().child(item.row(), item.column()-1);
      return QSqlRelationalTableModel::data(i, role);
    }
  } else
    return QSqlRelationalTableModel::data(item, role);
}

Qt::ItemFlags TimeTableModel::flags(const QModelIndex& index) const {
  if (index.column()==5) {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;  
  }
  return QSqlTableModel::flags(index);
}

void TimeTableModel::stopActivity() {
  const QDateTime d(QDateTime::currentDateTime());
  if (activity_running_) {
    current_activity_.setValue(TimeDatabase::T_END, d.toString(Qt::ISODate));
    insertRecord(-1, current_activity_);
    current_activity_.clearValues();
    activity_running_ = false;
    select();

    timer_.stop();
  }
}

void TimeTableModel::startActivity(const QString& name, const QString& category) {
  if (activity_running_) {
    stopActivity();
  }
  const QDateTime d(QDateTime::currentDateTime());

  current_activity_.setValue(TimeDatabase::T_START, d.toString(Qt::ISODate));
  current_activity_.setValue(TimeDatabase::T_ACTIVITY, name);
  current_activity_.setValue(TimeDatabase::T_CATEGORY, 1);
  activity_running_ = true;

  timer_.start(60*1000);
  minutes_passed_ = 0;
}
