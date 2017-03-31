#include "bpcategorygroupfilterproxy.h"

#include "blueprintlist.h"

bool BPCategoryGroupFilterProxy::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
  if (_categoryID <= 0)
    return true;

  QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
  int categoryID = index.data(BlueprintList::DataRoles::CategoryID).toInt();

  if (_categoryID == categoryID) {
    if (_groupID <= 0)
      return true;

    int groupID = index.data(BlueprintList::DataRoles::GroupID).toInt();
    if (groupID == _groupID)
      return true;
  }

  return false;
}

int BPCategoryGroupFilterProxy::groupID() const
{
  return _groupID;
}

void BPCategoryGroupFilterProxy::setGroupID(int groupID)
{
  _groupID = groupID;
  invalidateFilter();
}

BPCategoryGroupFilterProxy::BPCategoryGroupFilterProxy(QObject* parent) :
  QSortFilterProxyModel(parent)
{

}

int BPCategoryGroupFilterProxy::categoryID() const
{
  return _categoryID;
}

void BPCategoryGroupFilterProxy::setCategoryID(int value)
{
  _categoryID = value;
  invalidateFilter();
}
