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

#include "settings.h"
#include "helperfunctions.h"

#include <QColorDialog>
#include <QDebug>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QtConcurrent/QtConcurrent>

Settings::Settings(QSqlDatabase db, QWidget* parent): QDialog(parent), db_(db) {  
  ui_.setupUi(this);

  QSettings s("qamster");
  if (s.value(COLOR_CAT_IN_MAIN).toBool()) {
    ui_.categoryCBox->setCheckState(Qt::Checked);
  } else {
    ui_.categoryCBox->setCheckState(Qt::Unchecked);
  }

  QSqlQuery q("SELECT id,name,color FROM category", db);
  q.exec();
  while (q.next()) {
   QTableWidgetItem* cid =  new QTableWidgetItem(q.value(0).toString());
   QTableWidgetItem* name = new QTableWidgetItem(q.value(1).toString());
   const QColor c = TDBHelper::stringToColor(q.value(2).toString());
   QTableWidgetItem* ci = new QTableWidgetItem();
   ci->setBackground(c);

   const int row = ui_.categoryTable->rowCount();
   ui_.categoryTable->insertRow(row);
   ui_.categoryTable->setItem(row, 0, cid);
   ui_.categoryTable->setItem(row, 1, name);
   ui_.categoryTable->setItem(row, 2, ci);
  }

  QColor oc = s.value(TEXT_COLOR).value<QColor>();
  if (!oc.isValid())
    oc = QApplication::palette().text().color();
  textcolor_ = oc;
  setLabelTextColor(oc);

  connect(ui_.categoryTable, &QTableWidget::cellDoubleClicked, this, &Settings::cellDoubleClicked);
  connect(ui_.textColorButton, &QPushButton::pressed, this, &Settings::textColorButtonClicked);
}

void Settings::cellDoubleClicked(int row, int col) {
  if (col==2) {
    const QColor oc = ui_.categoryTable->item(row, col)->backgroundColor();
    const QColor c = QColorDialog::getColor(oc);
    if (c.isValid()) {
      ui_.categoryTable->item(row, col)->setBackground(c);
      changed_ = true;
    }
  }
}

void Settings::textColorButtonClicked() {
  QSettings s("qamster");
  const QColor col = QColorDialog::getColor(textcolor_);
  if (col.isValid()) {
    setLabelTextColor(col);
    textcolor_ = col;
    changed_ = true;
  }
}

void Settings::accept() {
  //save colors
  if (changed_) {
    QVariantList ids, colors;	//we save this before otherwise the widget could vanish while save runs
    for(int row=0; row<ui_.categoryTable->rowCount(); row++) {
      const QColor color = ui_.categoryTable->item(row, 2)->backgroundColor();
      const QString colorstring = TDBHelper::colorToString(color);
      const int cid = ui_.categoryTable->item(row,0)->text().toInt();
      ids << cid;
      colors << colorstring;
    }
    QtConcurrent::run(this, &Settings::saveColors, ids, colors);
  }

  QSettings s("qamster");
  s.setValue(COLOR_CAT_IN_MAIN, ui_.categoryCBox->checkState() == Qt::Checked);
  s.setValue(TEXT_COLOR, textcolor_);
  QDialog::accept();
}

void Settings::saveColors(const QVariantList& ids, const QVariantList& colors) {
  QSqlQuery q;
  q.prepare("UPDATE category SET color = ? WHERE id= ?");
  q.addBindValue(colors);
  q.addBindValue(ids);
  q.execBatch();
}

void Settings::setLabelTextColor(const QColor& col) {
  const QString style = QString("QLabel { color : %1; }").arg(col.name());
  ui_.textColorLabel->setStyleSheet(style);
}
