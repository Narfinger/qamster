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

int TimeDatabase::conNum = 0;

const int TimeDatabase::T_ID    = 0;
const int TimeDatabase::T_START = 1;
const int TimeDatabase::T_END   = 2;
const int TimeDatabase::T_ACTIVITY = 3;
const int TimeDatabase::T_CATEGORY = 4;
const int TimeDatabase::C_ID = 0;
const int TimeDatabase::C_NAME = 1;


TimeDatabase::TimeDatabase() {
  db_ = QSqlDatabase::addDatabase("QSQLITE");
  db_.setDatabaseName("tmp.db");
}

TimeDatabase::~TimeDatabase() {
  db_.close();
}

QSqlDatabase TimeDatabase::connect() {
  if (!created) createConnection();
  if (!db_.isOpen()) qDebug() << "Database not open";
  conNum++;
  QSqlDatabase newdb = QSqlDatabase::cloneDatabase(db_, QString(conNum));
  newdb.open();
  return newdb;
}

bool TimeDatabase::createConnection() {
  if (!db_.open()) return false;
  
  bool res = false;
  QSqlQuery q(db_);
  res = q.exec("CREATE TABLE if not exists category (id INTEGER PRIMARY KEY, name TEXT);");
  q.exec("CREATE TABLE if not exists time (id INTEGER PRIMARY KEY,"
				 "start TEXT,"
				 "end   TEXT,"
				 "activity TEXT,"
				 "category INTEGER,"
				 "FOREIGN KEY (category) REFERENCES category(id));");
  if (!res) qDebug() << q.lastError();
  created = true;
  return true;
}
