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

#ifndef HISTORY_H
#define HISTORY_H

#include <QSqlDatabase>
#include <QDate>
#include <QDialog>

#include "helperfunctions.h"

#include "ui_history.h"

class TimeTableModel;
class History : public QDialog
{
  Q_OBJECT
public:
  History(QSqlDatabase db, QWidget* parent = nullptr);
  ~History();

private:
  Ui::History ui_;
  QSqlDatabase db_;

  QSharedPointer<TimeTableModel> dd_ttm_;

private:
  void setupBarGraph();

  //black template magic, see comment in code
  template <typename Func=decltype(&TDBHelper::secsToQTime), typename Date>
  typename std::result_of<Func(int)>::type getTotal(const Date& start, const Date& end, Func f = TDBHelper::secsToQTime);

  const QPair<QDate,QDate> getWeek(const QDate& date);
  void activated(const QDate& date);
  void insertProgressBarIntoTable(QTableWidget* w, const QString& one, const QTime& time, const int totalsecs);
  void insertItemIntoTable(QTableWidget* w, const QString& one, const QString& two);
  void d_fillCategory(const QDate& date);
  void d_fillActivity(const QDate& date);
  void d_activated(const QDate& date);

  void dd_activated(const QDate& date);

  void w_activated(const QDate& date);

  void g_activated();
  void g_fillCateogory();
  void g_fillActivity();
};

#endif // HISTORY_H
