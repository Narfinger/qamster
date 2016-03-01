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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSqlDatabase>

#include "ui_settings.h"

static const char* COLOR_CAT_IN_MAIN = "colorcatinmain";
static const char* TEXT_COLOR = "textcolor";

class Settings : public QDialog
{
    Q_OBJECT
public:
    Settings(QSqlDatabase db, QWidget* parent = nullptr);

private slots:
  void cellDoubleClicked(int row, int col);
  void textColorButtonClicked();
  void accept() override;
  void saveColors(const QVariantList& ids, const QVariantList& colors);
    
private:
  Ui::Settings ui_;
  QSqlDatabase db_;
  bool changed_ = false;
  QColor textcolor_;

  void setLabelTextColor(const QColor& col);
};

#endif // SETTINGS_H
