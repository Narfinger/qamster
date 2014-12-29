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

#ifndef CATEGORYHISTOGRAM_H
#define CATEGORYHISTOGRAM_H

#include "histogram.h"

class CategoryHistogram : public Histogram
{
    Q_OBJECT
public:
  CategoryHistogram(QWidget* parent = 0);

  virtual void drawWeek(const QDate& start, const QDate& end);

protected:
  virtual void setupCategoryHistogram();      //this can only be called if we set a db and not in the constructor
  
private:
  QSqlDatabase db_;
  QVector<QCPBars*> bars_;	//the widget is responsible for destruction
  QVector<double> ticks_;
};

#endif // HISTOGRAM_H
