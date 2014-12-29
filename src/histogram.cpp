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

#include "histogram.h"

void Histogram::initialSetupHistogram() {
  clearGraphs();
  QBrush brush(palette().color(QPalette::Background));
  setBackground(brush);
  //legend
  legend->setVisible(true);
  legend->setBrush(palette().color(QPalette::Foreground));
  axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
  legend->setBrush(QColor(255, 255, 255, 200));

  xAxis->setAutoTicks(false);
  xAxis->setAutoTickLabels(false);
  xAxis->setSubTickCount(0);
  xAxis->setTickLength(0,10);
  xAxis->grid()->setVisible(false);
  xAxis->setRange(0,7);
  xAxis->setTickPen(palette().color(QPalette::Text));
  xAxis->setTickLabelColor(palette().color(QPalette::Text));
  xAxis->setBasePen(palette().color(QPalette::Text));

  yAxis->setTickPen(palette().color(QPalette::Text));
  yAxis->setTickLabelColor(palette().color(QPalette::Text));
  yAxis->setBasePen(palette().color(QPalette::Text));
}

