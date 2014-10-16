#include "qamster.h"


Qamster::Qamster() {
  ui_.setupUi(this);
  { //i don't know a different way. scoped because of tm would leak outside
    std::unique_ptr<QSqlRelationalTableModel> tm(new QSqlRelationalTableModel(this, tdb_.connect()));
    rtm_ = std::move(tm);
  }

  rtm_->setTable("time");
  rtm_->setRelation(3, QSqlRelation("activity", "id", "name"));
  ui_.tableView->setModel(rtm_.get());
  ui_.tableView->setColumnHidden(0, true);
  //rtm_->setHeaderData(0, Qt::Horizontal, "tmp1");
  rtm_->setHeaderData(1, Qt::Horizontal, "Start Time");
  rtm_->setHeaderData(2, Qt::Horizontal, "End Time");
  rtm_->setHeaderData(3, Qt::Horizontal, "Activity");
  rtm_->select();
  qDebug() << "done";

  QSqlQuery q(tdb_.connect());
  q.exec("select * from time;");
  while (q.next()) {
    qDebug() << q.value(0) << q.value(1);
  }
}
