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

#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include "ui_editdialog.h"
#include <memory>

#include "activitycompletermodel.h"

class TimeTableModel;


class EditDialog : public QDialog
{
    Q_OBJECT
public:
  EditDialog(QSharedPointer<TimeTableModel> ptr, QSqlDatabase db, const QModelIndex& index);
  bool isRunning() const { return running_; };

private:
    Ui::EditDialog ui_;
    
    std::unique_ptr<ActivityCompleterModel> acm_;
    QSharedPointer<TimeTableModel> ttm_;
    QSqlDatabase db_;
    const QModelIndex index_;
    bool running_ = false;	//are we editing the currently running one?
    bool changed_ = false;
    QDateTime start_min_;
    QDateTime start_max_;
    QDateTime end_min_;
    QDateTime end_max_;

    bool checkBounds() const;
    
private slots:
    void accept() override;
};

#endif // EDITDIALOG_H
