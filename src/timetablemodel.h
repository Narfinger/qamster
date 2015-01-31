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

#ifndef TIMETABLEMODEL_H
#define TIMETABLEMODEL_H

#include <QDate>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QTimer>

class TimeTableModel : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
  TimeTableModel(QObject* parent, QSqlDatabase db);
  ~TimeTableModel();
  int columnCount(const QModelIndex& parent = QModelIndex()) const override { Q_UNUSED(parent); return 7; };
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& item, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
  bool setData(const QModelIndex& index, const QDateTime& d, int role = Qt::EditRole);	//overload setData for datetime because we have custom datestring format
  Qt::ItemFlags flags(const QModelIndex& index) const override;

  void stopActivity();
  void startActivity(const QString& name, const QString& category);
  const QString getTodaysStatusbarText() const;
  void update(const bool force = false);	//do select and update the filter if necessary
  const QStringList categories() const;
  int categoryIdForIndex(const QModelIndex& index) const;
  void setCategory(const QModelIndex& index, const int c);

signals:
  void minutesPassed(const int minutes);

private:
  bool activity_running_ = false;
  QSqlRecord current_activity_;
  QTimer timer_;
  int minutes_passed_ = 0;
  QDate currentdate_;	//we save the date last time to check if we need to update the filter in update()

  QVariant runningActivityData(const QModelIndex& item, int role = Qt::DisplayRole) const;	//when we have the running row use this data
  QVariant backgroundColorRoleData(const QModelIndex& index, int role) const;
  QVariant textColorRoleData(const QModelIndex& index, int role) const;
};

#endif // TIMETABLEMODEL_H
