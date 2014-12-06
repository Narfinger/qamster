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

#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <QSqlQuery>
#include <QTime>

namespace TDBHelper {
  const QTime secsToQTime(const int seconds);
  const QString secsToQString(const int seconds);

  //functions that replace %1 and %2 with the start and end time
  QSqlQuery queryTimeSubstitution(const QString& query, const QSqlDatabase& db, const QDateTime& start, const QDateTime& end);
  QSqlQuery queryTimeSubstitution(const QString& query, const QSqlDatabase& db, const QDate& start, const QDate& end);
  QSqlQuery queryTimeSubstitution(const QString& query, const QSqlDatabase& db, const QDate& date);
}


#endif // HELPERFUNCTIONS_H
