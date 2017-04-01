#include "blueprintsfilterproxy.h"

#include <QPair>
#include <QDebug>

#include "blueprintlist.h"


BlueprintsFilterProxy::BlueprintsFilterProxy(QObject* parent) :
  QSortFilterProxyModel(parent)
{
  setDynamicSortFilter(true);
}


bool BlueprintsFilterProxy::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
  if (filter_.isNull())
    return true;

  QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

  if (index.data(BlueprintList::IsHeaderRole).toBool() == true)
    return true;

  bool res = filter_->validate(index);
  return res;
}

QSharedPointer<BpFilter> BlueprintsFilterProxy::filter() const
{
  return filter_;
}

void BlueprintsFilterProxy::setFilter(QSharedPointer<BpFilter> filter)
{
  if (!filter_.isNull())
    disconnect(filter_.data(), 0, this, 0);

  filter_ = filter;
  connect(filter_.data(), &BpFilter::valueChanged, [this]() {
    invalidateFilter();
    emit invalidated();
  });

  invalidateFilter();
  emit invalidated();
}
