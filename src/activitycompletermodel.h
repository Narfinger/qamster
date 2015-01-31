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

#ifndef ACTIVITYCOMPLETERMODEL_H
#define ACTIVITYCOMPLETERMODEL_H

#include <QDebug>
#include <QSqlQuery>
#include <QSqlQueryModel>

class ActivityCompleterModel : public QSqlQueryModel
{
  Q_OBJECT
public:
  ActivityCompleterModel(QSqlDatabase db, QObject* parent = nullptr);
  virtual QVariant data(const QModelIndex& item, int role = Qt::DisplayRole) const override { return  QSqlQueryModel::data(item, role); }
};

class ActivityCategoryCompleterModel : public QSqlQueryModel
{
    Q_OBJECT
public:
  ActivityCategoryCompleterModel(QSqlDatabase db, QObject* parent = nullptr);
  virtual QVariant data(const QModelIndex& item, int role = Qt::DisplayRole) const override;
  
  void select();	//refreshes the query

private:
  QSqlDatabase db_;
};

#endif // ACTIVITYCOMPLTERMODEL_H
