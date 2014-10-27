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

#include "activitycompletermodel.h"

#include "timedatabase.h"

ActivityCompleterModel::ActivityCompleterModel(QSqlDatabase db, QObject* parent) : QSqlQueryModel(parent), db_(db) {
  select();
}

QVariant ActivityCompleterModel::data(const QModelIndex& item, int role) const {
  if (!item.isValid() ||
     (role != Qt::DisplayRole && role != Qt::EditRole)) {
    return QSqlQueryModel::data(item, role);
  }
  const QString activity = QSqlQueryModel::data(sibling(item.row(),0,item)).toString();
  const QString category = QSqlQueryModel::data(sibling(item.row(),1,item)).toString();
  const QString final = activity + "@" + category;
  return QVariant(final);
}

void ActivityCompleterModel::select() {
  const QSqlQuery q("SELECT DISTINCT time.activity AS name, category.name AS category \
		    FROM time inner join category ON time.category=category.id ORDER BY datetime(time.end)",
		    db_);
  setQuery(q);
}
