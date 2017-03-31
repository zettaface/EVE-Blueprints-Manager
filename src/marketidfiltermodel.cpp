#include "marketidfiltermodel.h"

MarketIDFilterModel::MarketIDFilterModel(QObject* parent) : QSortFilterProxyModel(parent)
{
  _marketID = -1;
  _sourceModel = nullptr;
}

void MarketIDFilterModel::setBPList(BlueprintList* list)
{
  _sourceModel = list;
}

bool MarketIDFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
  if (_marketID < 0)
    return true;

  if (source_parent.isValid()) {
    const Blueprint& bp = _sourceModel->blueprint(source_parent.row(), source_row);
    if (bp.prodMarketGroups.contains(_marketID))
      return true;
  } else {
    const Blueprint& bp = _sourceModel->blueprintHeader(source_row);
    if (bp.prodMarketGroups.contains(_marketID))
      return true;
  }

  return false;
}

void MarketIDFilterModel::setMarketID(int marketID)
{
  _marketID = marketID;
  invalidateFilter();
}
