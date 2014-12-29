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

#include <QSqlQuery>

#include "category-histogram.h"
#include "timedatabase.h"
#include "helperfunctions.h"

CategoryHistogram::CategoryHistogram(QWidget* parent): Histogram(parent) { 
}

void CategoryHistogram::setupCategoryHistogram() {
  initialSetupHistogram();

  QSqlQuery q("SELECT name,color FROM category", db_);
  q.exec();
  while (q.next()) {
    QCPBars* bar = new QCPBars(xAxis, yAxis);
    bar->setWidth(0.25);
    const QColor c = TDBHelper::stringToColor(q.value(1).toString());
    QPen p(c);
    p.setStyle(Qt::NoPen);
    bar->setPen(p);
    bar->setBrush(c);
    bar->rescaleAxes();
    bar->setName(q.value(0).toString());
    addPlottable(bar);
    bars_ << bar;
  }
  for(int i = 1; i<bars_.size(); i++) {
    bars_[i]->moveAbove(bars_[i-1]);
  }
  ticks_ << 0.5 << 1.0 << 1.5 << 2.0 << 2.5 << 3.0 << 3.5;
  xAxis->setTickVector(ticks_);
  QVector<QString> labels;
  labels << "Mon" << "Tue" << "Wed" << "Thu" << "Fri" << "Sat" << "Sun";
  xAxis->setTickVectorLabels(labels);
  yAxis->setRange(0,10);
}

void CategoryHistogram::drawWeek(const QDate& start, const QDate& end) {
  Q_UNUSED(end)
  QVector<QVector<double> > b;

  QSqlQuery cat("SELECT id FROM category", db_);
  cat.exec();
  QVector<int> cid;
  while (cat.next()) {
    cid << cat.value(0).toInt();
    b.append(QVector<double>());
  }
  
  for(int i = 0; i < 7; i++) {
    const QDateTime t_start = QDateTime(start).addDays(i);
    const QDateTime t_end = QDateTime(start).addDays(i+1).addSecs(-1);

    for(const int j : cid) {
      //fill bars
      QString qstring("SELECT sum(strftime('%s', end) - strftime('%s', start)) AS sum FROM \
		      time WHERE start>='%1' AND end<='%2' AND category=%3");
      qstring = qstring.arg(t_start.toString(TimeDatabase::DATEFORMAT)).arg(t_end.toString(TimeDatabase::DATEFORMAT)).arg(j);
      QSqlQuery cquery(qstring, db_);
      cquery.exec();
      cquery.next();

      const QTime t = TDBHelper::secsToQTime(cquery.value(0).toInt());
      const double hours = t.hour();
      const double minutes = ((double)t.minute() / 60l);
      b[j-1] << hours + minutes;
    }
  }

  for(int i=0; i< bars_.size(); i++) {
    QCPBars* bar = bars_[i];
    bar->clearData();
    bar->addData(ticks_, b[i]);
  }
  this->replot();
}

