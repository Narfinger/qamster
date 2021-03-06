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

#ifndef TIMEDATABASE_H
#define TIMEDATABASE_H

#include <QtSql/QtSql>

class TimeDatabase : public QObject
{
Q_OBJECT

public:
  TimeDatabase();
  ~TimeDatabase();
  QSqlDatabase connect();

  //db shortcuts
  static const int T_ID;
  static const int T_START;
  static const int T_END;
  static const int T_ACTIVITY;
  static const int T_CATEGORY;
  static const int C_ID;
  static const int C_NAME;
  static const int C_COLOR;

  static const QString DATEFORMAT;

private:  
  bool created = false;
  QSqlDatabase db_;
  
  bool createConnection();
  
  static int conNum;
};


#endif // TIMEDATABASE_H
