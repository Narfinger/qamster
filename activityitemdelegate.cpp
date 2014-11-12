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

#include "activityitemdelegate.h"
#include "timedatabase.h"

#include <QApplication>
#include <QPainter>

void ActivityItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  QStyleOptionViewItem o = option;
  initStyleOption(&o, index);

  if (index.column()== TimeDatabase::T_START || index.column() == TimeDatabase::T_END) {
    painter->save();
    QStyle* style = QApplication::style();
    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &option);
    const QDateTime d = QDateTime::fromString(o.text, Qt::ISODate);
    painter->drawText(textRect, o.displayAlignment, d.toString("h:mm"));
    painter->restore();
  } else
    QStyledItemDelegate::paint(painter, o, index); 
}