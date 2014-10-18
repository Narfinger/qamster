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

#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QTimer>

class TimeTableModel : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
  TimeTableModel(QObject* parent, QSqlDatabase db);
  int columnCount(const QModelIndex& parent = QModelIndex()) const { Q_UNUSED(parent); return 6; };
  QVariant data(const QModelIndex& item, int role = Qt::DisplayRole) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;

  void stopActivity();
  void startActivity(const QString& name, const QString& category);

signals:
  void minutesPassed(const int minutes);

private:
  bool activity_running_ = false;
  QSqlRecord current_activity_;
  QTimer timer_;
  int minutes_passed_ = 0;
};

#endif // TIMETABLEMODEL_H
