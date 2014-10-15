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

#include "timedatabase.h"

TimeDatabase::TimeDatabase()
{
}

TimeDatabase::~TimeDatabase()
{
}

QSqlDatabase TimeDatabase::connect() {
  if (!created) createConnection();
  return QSqlDatabase::cloneDatabase(db_, "OutsideConn");
}

bool TimeDatabase::createConnection() {
  db_ = QSqlDatabase::addDatabase("QSQLITE");
  db_.setDatabaseName("tmp.db");
  if (!db_.open()) return false;
  
  bool res = false;
  QSqlQuery q(db_);
  res = q.exec("CREATE TABLE if not exists category (id INTEGER PRIMARY KEY, name TEXT);");
  
  q.exec("CREATE TABLE if not exists activity (id INTEGER PRIMARY KEY,"
				"name TEXT,"
				"category INTEGER,"
				"FOREIGN KEY (category) REFERENCES category(id));");
  if (!res) qDebug() << q.lastError();
  q.exec("CREATE TABLE if not exists time (id integer primary key,"
				 "start text,"
				 "end   text,"
				 "activity INTEGER,"
				 "FOREIGN KEY (activity) REFERENCES activity(id));");
  if (!res) qDebug() << q.lastError();
  created = true;
  return true;
}
