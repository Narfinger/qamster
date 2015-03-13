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
#include <QCompleter>
#include <QTime>
#include <QMessageBox>

#include "timedatabase.h"
#include "timetablemodel.h"

EditDialog::EditDialog(QSharedPointer<TimeTableModel> ptr, QSqlDatabase db, const QModelIndex& index) : ttm_(ptr), db_(db), index_(index) {
   ui_.setupUi(this);
   {
     std::unique_ptr<ActivityCompleterModel> up(new ActivityCompleterModel(db_, this));
     acm_ = std::move(up);
   }
   QCompleter* c = new QCompleter(acm_.get(), ui_.activityEdit);
   c->setCaseSensitivity(Qt::CaseInsensitive);
   c->setCompletionMode(QCompleter::PopupCompletion);
   ui_.activityEdit->setCompleter(c);

   //fill data
   const QString activity = ttm_->data(index.sibling(index.row(), TimeDatabase::T_ACTIVITY)).toString();
   ui_.activityEdit->setText(activity);

   const QDateTime start = ttm_->data(index.sibling(index.row(), TimeDatabase::T_START)).toDateTime();
   const QDateTime lastEnd = ttm_->data(index.sibling(index.row() -1, TimeDatabase::T_END)).toDateTime();
   const QDateTime end = ttm_->data(index.sibling(index.row(), TimeDatabase::T_END)).toDateTime();
   const QDateTime nextStart = ttm_->data(index.sibling(index.row() +1, TimeDatabase::T_START)).toDateTime();

   running_ = !nextStart.isValid();

   ui_.startTimeEdit->setDateTime(start);
   ui_.endTimeEdit->setDateTime(end);
   end_min_ = start;
   //ui_.endTimeEdit->setMinimumDateTime(start);
   start_max_ = nextStart;
   //ui_.startTimeEdit->setMaximumDateTime(nextStart);

   if (lastEnd.isValid()) {	//if this is not the first element and on the same day
     start_min_ = lastEnd;
     //ui_.startTimeEdit->setMinimumDateTime(lastEnd);
   } else {
     QDateTime d(start);
     d.setTime(QTime(0,0,0));
     start_min_ = d;
     //ui_.startTimeEdit->setMinimumDateTime(d);
   }
   if (nextStart.isValid()) {
     end_min_ = nextStart;
     //ui_.endTimeEdit->setMaximumDateTime(nextStart);
   } else {
     const QDateTime d = QDateTime::currentDateTime();
     end_max_ = d;
     //ui_.endTimeEdit->setMaximumDateTime(d);
   }

   ui_.categoryCBox->addItems(ptr->categories());
   ui_.categoryCBox->setCurrentIndex(ttm_->categoryIdForIndex(index));   

   if (running_) {
     ui_.endTimeEdit->setEnabled(false);
     const QDateTime d = QDateTime(QDateTime::currentDateTime().addDays(1));
     ui_.startTimeEdit->setMaximumDateTime(d);
   }

   connect(ui_.activityEdit, &QLineEdit::textEdited, [=]() { changed_ = true; });
   connect(ui_.startTimeEdit, &QAbstractSpinBox::editingFinished, [=]() { changed_ = true; });
   connect(ui_.endTimeEdit, &QAbstractSpinBox::editingFinished, [=]() { changed_ = true; });
   connect(ui_.categoryCBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=]() { changed_ = true; });
}

bool EditDialog::checkBounds() const {
  const bool start = ui_.startTimeEdit->time() >= start_min_.time() && ui_.startTimeEdit->time() <= start_max_.time();
  const bool end   = ui_.endTimeEdit->time() >= end_min_.time() && ui_.endTimeEdit->time() <= end_max_.time();
  return start && end;
}


void EditDialog::accept() {
  if (!checkBounds()) {
    QMessageBox::critical(this, "Not saved", "The bounds where not valid, we did not change the data!");
    reject();
    return;
  }

  if (changed_) {
    ttm_->setData(index_.sibling(index_.row(), TimeDatabase::T_ACTIVITY), ui_.activityEdit->text());

    QDateTime start = ttm_->data(index_.sibling(index_.row(), TimeDatabase::T_START)).toDateTime();
    start.setTime(ui_.startTimeEdit->time());
    ttm_->setData(index_.sibling(index_.row(), TimeDatabase::T_START), start);

    if (!running_) {
      QDateTime end = ttm_->data(index_.sibling(index_.row(), TimeDatabase::T_END)).toDateTime();
      end.setTime(ui_.endTimeEdit->time());
      ttm_->setData(index_.sibling(index_.row(), TimeDatabase::T_END), end);
    }

    ttm_->setCategory(index_.sibling(index_.row(), TimeDatabase::T_CATEGORY), ui_.categoryCBox->currentIndex());

    ttm_->submitAll();
  }
  QDialog::accept();
}

