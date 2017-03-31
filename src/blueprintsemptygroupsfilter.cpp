#include "blueprintsemptygroupsfilter.h"

#include <QDebug>

void BlueprintsEmptyGroupsFilter::setEnabled(bool enabled)
{
  enabled_ = enabled;
  invalidateFilter();
}

void BlueprintsEmptyGroupsFilter::setDisabled(bool disabled)
{
  setEnabled(!disabled);
}

bool BlueprintsEmptyGroupsFilter::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
  if (!enabled_)
    return true;

  if (source_parent.isValid())
    return true;

  QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
  if (sourceModel()->rowCount(index) > 0)
    return true;

  return false;
}
