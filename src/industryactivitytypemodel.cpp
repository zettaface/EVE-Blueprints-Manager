#include "industryactivitytypemodel.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "eve/staticdata.h"

QVariant IndustryActivityTypeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QAbstractListModel::headerData(section, orientation, role);
}

int IndustryActivityTypeModel::rowCount(const QModelIndex& parent) const
{
  return data_.size();
}

QVariant IndustryActivityTypeModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  switch(role) {
    case Qt::DisplayRole:
      return data_[index.row()].name;
    case Qt::UserRole:
      return data_[index.row()].id;
    default:
      return QVariant();
  }

  return QVariant();
}

void IndustryActivityTypeModel::fetchFromSDE()
{
  QSqlDatabase db = eve::StaticData::getConnection();
  if (!db.isOpen()) {
    qDebug() << db.lastError();
    return;
  }

  beginResetModel();
  data_.clear();

  IndustryActivity anyActivity {"Any activity", "All active jobs", -1};
  data_.push_back(anyActivity);

  QSqlQuery query("SELECT * FROM ramActivities WHERE published=1 ORDER BY activityID ASC", db);
  qDebug() << query.lastError();

  while(query.next()) {
    IndustryActivity i;
    i.id = query.value("activityID").toInt();
    i.name = query.value("activityName").toString();
    i.desc = query.value("description").toString();

    data_.push_back(i);
  }

  endResetModel();
}

QString IndustryActivityTypeModel::activityName(int activityID) const
{
  for (int i = 0; i < data_.size(); i++)
    if (data_[i].id == activityID)
      return data_[i].name;

  return "Unknown activity";
}
