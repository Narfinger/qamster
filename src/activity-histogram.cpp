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

void ActivityHistogram::setupHistogram() {
  initialSetupHistogram();
  //this is an ugly hack for the moment
  QStringList colors = { "red", "green", "blue", "darkRed", "darkGreen", "darkBlue", "cyan", "magenta", "yellow", "darkCyan", "darkMagenta", "darkYellow" , "black"};
  QStringListIterator cit(colors);

  QSqlQuery q("SELECT DISTINCT activity FROM time ORDER BY activity", db_);
  q.exec();
  while(q.next()) {
    const QString activity = q.value(0).toString();
    QCPBars* bar = new QCPBars(xAxis, yAxis);
    bar->setWidth(0.25);
    QColor c;
    if (cit.hasNext()) {
      c = QColor(cit.next());
    } else {
     c = Qt::white; 
    }
    QPen p(c);
    p.setStyle(Qt::NoPen);
    bar->setPen(p);
    bar->setBrush(c);
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

  QMap<QString, QCPBars*>::iterator current, previous;
  current = ++(bars__.begin());
  previous = bars__.begin();
  for (; current != bars__.end(); ++current, ++previous) {
    QCPBars* c = current.value();
    QCPBars* p = previous.value();
    qDebug() << "moving:" << current.key() << "above" << previous.key();
    c->moveAbove(p);
  }
}

void ActivityHistogram::drawWeek(const QDate& start, const QDate& end) {

  QHash<QPair<QString, int>, double> b;     //activities, days

  QString qstring("SELECT t.activity,sum(strftime('%s', end) - strftime('%s', start)) AS sum FROM (SELECT DISTINCT activity FROM time) AS a \
		    LEFT JOIN time AS t ON t.activity = a.activity WHERE start>='%1' AND end<='%2' GROUP BY a.activity");
  
  for (int i = 0; i < 7; i++) {
    const QDateTime t_start = QDateTime(start).addDays(i);
    const QDateTime t_end = QDateTime(start).addDays(i+1).addSecs(-1);
    
    QSqlQuery cquery(qstring.arg(t_start.toString(TimeDatabase::DATEFORMAT)).arg(t_end.toString(TimeDatabase::DATEFORMAT)), db_);
    cquery.exec();
    
    while (cquery.next()) {
      const QString activity = cquery.value(0).toString();
      const QTime t = TDBHelper::secsToQTime(cquery.value(1).toInt()); 
      const double hours = t.hour();
      const double minutes = ((double)t.minute() / 60l);
      const QPair<QString, int> p(activity, i);
      b.insert(p, hours + minutes);
    }
  }

  
  for (QMap<QString, QCPBars*>::iterator j=bars__.begin(); j!=bars__.end(); ++j) {
    QCPBars* bar = j.value();
    bar->clearData();

    bool nonzero = false;
    QVector<double> v;
    for(int i=0; i<7; i++) {
      QPair<QString, int> p(j.key(), i);
      v << b.value(p);
      if (b.value(p) != 0.0)
        nonzero = true;
    }
    bar->setData(ticks_, v);
    //redo legend
    if (!nonzero)
      bar->removeFromLegend();
    else
      bar->addToLegend();
  }
  this->replot();
}
