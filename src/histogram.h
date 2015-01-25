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

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QSqlDatabase>

#include "../lib/qcustomplot.h"

class Histogram : public QCustomPlot
{
    Q_OBJECT
public:
  Histogram(QWidget* parent = 0) : QCustomPlot(parent) {};

  void setDb(QSqlDatabase db) { db_ = db; this->setupHistogram(); };
  virtual void drawWeek(const QDate& start, const QDate& end) = 0;
  
protected :
  virtual void setupHistogram() = 0;        //this can only be called if we set a db and not in the constructor
  void initialSetupHistogram();

  QSqlDatabase db_;
  QVector<QCPBars*> bars_;      //the widget is responsible for destruction
  QVector<double> ticks_;
};

#endif // HISTOGRAM_H
