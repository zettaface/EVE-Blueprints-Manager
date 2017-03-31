#include "bpgroupsmodel.h"

namespace eve {

QVector<invGroup> BPGroupsModel::data_;

BPGroupsModel::BPGroupsModel(QObject* parent) :
  QAbstractListModel(parent)
{
  allGroup.name = "All";
  allGroup.categoryID = -1;
  allGroup.ID = -1;
}


int BPGroupsModel::rowCount(const QModelIndex& parent) const
{
  return data_.size() + 1;
}

int BPGroupsModel::rowByGroupId(int ID) const
{
  if (ID == allGroup.ID)
    return 0;

  for (int i = 0; i < data_.size(); i++) {
    if (ID == data_[i].ID)
      return i + 1;
  }

  return -1;
}

QVariant BPGroupsModel::data(const QModelIndex& index, int role) const
{
  switch(role) {
    case Qt::DisplayRole:
      if (index.row() > 0)
        return data_[index.row() - 1].name;

      return allGroup.name;
    case GroupID:
      if (index.row() > 0)
        return data_[index.row() - 1].ID;

      return allGroup.ID;
    case CategoryID:
      if (index.row() > 0)
        return data_[index.row() - 1].categoryID;

      return allGroup.categoryID;
    default:
      break;
  }

  return QVariant();
}

invGroup BPGroupsModel::groupByID(int ID) const
{
  if (ID == -1)
    return allGroup;

  auto it = std::find_if(data_.begin(), data_.end(), [ID](const invGroup & g) {
    if (g.ID == ID)
      return true;

    return false;
  });

  if (it == data_.end())
    return invGroup();

  return *it;
}

}
