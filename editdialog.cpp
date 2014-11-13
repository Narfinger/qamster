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

#include "editdialog.h"
#include <QTime>

#include "timedatabase.h"
#include "timetablemodel.h"

EditDialog::EditDialog(QSharedPointer<TimeTableModel> ptr, QSqlDatabase db, const QModelIndex& index) : ttm_(ptr), db_(db), index_(index) {
   ui_.setupUi(this);

   //fill data
   const QString activity = ttm_->data(index.sibling(index.row(), TimeDatabase::T_ACTIVITY)).toString();
   ui_.activityEdit->setText(activity);

   const QDateTime start = ttm_->data(index.sibling(index.row(), TimeDatabase::T_START)).toDateTime();
   const QDateTime lastEnd = ttm_->data(index.sibling(index.row() -1, TimeDatabase::T_END)).toDateTime();
   ui_.startTimeEdit->setDateTime(start);
   if (lastEnd.isValid() && start.date() == lastEnd.date()) {	//if this is not the first element and on the same day
     ui_.startTimeEdit->setMinimumDateTime(lastEnd);
   }

   const QDateTime end = ttm_->data(index.sibling(index.row(), TimeDatabase::T_END)).toDateTime();
   const QDateTime nextStart = ttm_->data(index.sibling(index.row() +1, TimeDatabase::T_START)).toDateTime();
   ui_.endTimeEdit->setDateTime(end);
   if (nextStart.isValid() && end.date() == start.date()) {
     ui_.endTimeEdit->setMaximumDateTime(nextStart);
   }

   ui_.categoryCBox->addItems(ptr->categories());

   ui_.categoryCBox->setCurrentIndex(ttm_->categoryIdForIndex(index)-1); //indices start at one cbox starts at zero
   
   connect(ui_.activityEdit, &QLineEdit::textEdited, [=]() { changed = true; });
   connect(ui_.startTimeEdit, &QAbstractSpinBox::editingFinished, [=]() { changed = true; });
   connect(ui_.endTimeEdit, &QAbstractSpinBox::editingFinished, [=]() { changed = true; });
   connect(ui_.categoryCBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=]() { changed = true; });
}

void EditDialog::accept() {
  qDebug() << "sorry no modification yet";

  QDialog::accept();
}

