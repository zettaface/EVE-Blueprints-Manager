#include "bpcategoriesmodel.h"

namespace eve {

QVector<invCategory> BPCategoriesModel::data_;

BPCategoriesModel::BPCategoriesModel(QObject* parent) :
  QAbstractListModel(parent)
{
  allCategory.ID = -1;
  allCategory.name = "All";
}

int BPCategoriesModel::rowCount(const QModelIndex& parent) const
{
  return data_.size() + 1;
}

int BPCategoriesModel::rowByCategoryId(int ID) const
{
  if (ID == allCategory.ID)
    return 0;

  for (int i = 0; i < data_.size(); i++) {
    if (data_[i].ID == ID)
      return i + 1;
  }

  return -1;
}

QVariant BPCategoriesModel::data(const QModelIndex& index, int role) const
{
  switch(role) {
    case Qt::DisplayRole:
      if (index.row() > 0)
        return data_[index.row() - 1].name;

      return allCategory.name;
    case Qt::ToolTipRole:
      if (index.row() > 0)
        return data_[index.row() - 1].name;

      return allCategory.name;
    case Qt::UserRole:
      if (index.row() > 0)
        return data_[index.row() - 1].ID;

      return allCategory.ID;
    default:
      break;
  }

  return QVariant();
}

invCategory BPCategoriesModel::categoryByID(int ID) const
{
  if (ID == -1)
    return allCategory;

  auto it = std::find_if(data_.begin(), data_.end(), [ID](const invCategory & c) {
    if (c.ID == ID)
      return true;

    return false;
  });

  if (it == data_.end())
    return invCategory();

  return *it;
}

}
