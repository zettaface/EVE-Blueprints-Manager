#include "categoryfilterproxymodel.h"

#include <QDebug>

int CategoryFilterProxyModel::categoryID() const
{
  return _categoryID;
}

void CategoryFilterProxyModel::setCategoryID(int ID)
{
  _categoryID = ID;
  invalidateFilter();
}

bool CategoryFilterProxyModel::isAllShowed() const
{
  return _all;
}

void CategoryFilterProxyModel::setShowAll(bool all)
{
  _all = all;
}

bool CategoryFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
  QModelIndex index = sourceModel()->index(source_row, filterKeyColumn(), source_parent);
  int id = index.data(filterRole()).toInt();

  if (index.data(filterRole()).toInt() == _categoryID)
    return true;

  if (_all && index.data(filterRole()).toInt() < 0)
    return true;

  return false;
}
