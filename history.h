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

#include "ui_history.h"

class TimeTableModel;
class History : public QDialog
{
  Q_OBJECT
public:
  History(QSqlDatabase db, QWidget* parent = 0);

private:
  Ui::History ui_;
  QSqlDatabase db_;

private:
  void d_fillCategory(const QDate& date);
  void d_activated(const QDate& date);
};

#endif // HISTORY_H
