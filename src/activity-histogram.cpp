/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014
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

#include "activity-histogram.h"
#include "timedatabase.h"
#include "helperfunctions.h"

#include <QDebug>
#include <QSqlQuery>

void ActivityHistogram::setupCategoryHistogram() {
  initialSetupHistogram();

  QSqlQuery q("SELECT DISTINCT activity FROM time ORDER BY", db_);
  q.exec();
  while(q.next()) {
    const QString activity = q.value(0).toString();
    QCPBars* bar = new QCPBars(xAxis, yAxis);
    bar->setWidth(0.25);
    bar->rescaleAxes();
    bar->setName(activity);
    addPlottable(bar);
    bars__.insert(activity, bar);
  }

  ticks_ << 0.5 << 1.0 << 1.5 << 2.0 << 2.5 << 3.0 << 3.5;
  xAxis->setTickVector(ticks_);
  QVector<QString> labels;
  labels << "Mon" << "Tue" << "Wed" << "Thu" << "Fri" << "Sat" << "Sun";
  xAxis->setTickVectorLabels(labels);
  yAxis->setRange(0,10);
  legend->setVisible(false);
}

void ActivityHistogram::drawWeek(const QDate& start, const QDate& end) {
  QHash<QString, QVector<double> > b;     //activities, days
  
  for (int i = 0; i < 7; i++) {
    const QDateTime t_start = QDateTime(start).addDays(i);
    const QDateTime t_end = QDateTime(start).addDays(i+1).addSecs(-1);

    QHash<QString, QCPBars*>::const_iterator j = bars__.constBegin();
    while (j != bars__.constEnd()) {
      QString qstring("SELECT sum(strftime('%s', end) - strftime('%s', start)) AS sum FROM \
                      time WHERE start>='%1' AND end<='%2' AND activity=%3");
      qstring = qstring.arg(t_start.toString(TimeDatabase::DATEFORMAT)).arg(t_end.toString(TimeDatabase::DATEFORMAT)).arg(j.key());
      QSqlQuery cquery(qstring, db_);
      cquery.exec();
      cquery.next();
      
      const QTime t = TDBHelper::secsToQTime(cquery.value(0).toInt());
      const double hours = t.hour();
      const double minutes = ((double)t.minute() / 60l);
      //values.insert(i.key(), hours + minutes);
      ++j;
    }
  }
}
