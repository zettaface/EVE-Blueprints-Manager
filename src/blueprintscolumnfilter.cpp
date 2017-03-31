#include "blueprintscolumnfilter.h"

BlueprintsColumnFilter::BlueprintsColumnFilter(QObject* parent) :
  BlueprintsColumnFilter({}, parent)
{

}

BlueprintsColumnFilter::BlueprintsColumnFilter(const QSet<int>& filteredColumns, QObject* parent) :
  QSortFilterProxyModel(parent), filteredColumns_(filteredColumns)
{

}

void BlueprintsColumnFilter::filterColumn(int column, bool filter)
{
  if (filter)
    filteredColumns_.insert(column);
  else
    filteredColumns_.remove(column);

  invalidateFilter();
}

QSet<int> BlueprintsColumnFilter::filteredColumns() const
{
  return filteredColumns_;
}

void BlueprintsColumnFilter::setFilteredColumns(const QSet<int>& filteredColumns)
{
  filteredColumns_ = filteredColumns;
}


bool BlueprintsColumnFilter::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const
{
  if (filteredColumns_.contains(source_column))
    return false;

  return true;
}
