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

#include "helperfunctions.h"
#include "timedatabase.h"

#include <QDebug>

const QTime TDBHelper::secsToQTime(const int seconds) {
  const int s = seconds % 60;
  const int m = (seconds / 60) % 60;
  const int h = ((seconds / 60) / 60);
  if (h>24) qDebug() << "Error in conversion, hours =" << h;
  return QTime(h,m,s);
}

const QString TDBHelper::secsToQString(const int seconds) {
  //const int s = seconds % 60;
  const int m = (seconds / 60) % 60;
  const int h = (seconds / 60) / 60;
  return QString("%1:%2").arg(h).arg(m, 2, 10, QChar('0'));
}

QSqlQuery TDBHelper::queryTimeSubstitution(const QString& query, const QSqlDatabase& db, const QDate& date) { 
  return queryTimeSubstitution(query, db, date, date);
}

QSqlQuery TDBHelper::queryTimeSubstitution(const QString& query, const QSqlDatabase& db, const QDate& start, const QDate& end) {
  const QDateTime startd(start);
  const QDateTime endd = QDateTime(end.addDays(1)).addSecs(-1);
  return queryTimeSubstitution(query, db, startd, endd);
}

QSqlQuery TDBHelper::queryTimeSubstitution(const QString& query, const QSqlDatabase& db, const QDateTime& start, const QDateTime& end) {
  const QString querystring = query.arg(start.toString(TimeDatabase::DATEFORMAT)).arg(end.toString(TimeDatabase::DATEFORMAT));
  QSqlQuery q(querystring, db);
  return q;
}

