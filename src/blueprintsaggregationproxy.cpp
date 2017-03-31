#include "blueprintsaggregationproxy.h"

#include <QDebug>

#include "blueprint.h"
#include "blueprintlist.h"

QVariant BlueprintsAggregationProxy::data(const QModelIndex& index, int role) const
{
  if (role == Qt::DisplayRole) {
    if (this->hasChildren(index)) {
      if (index.column() == Blueprint::Runs) {
        if (data_.isEmpty())
          gatherAggregationData();

        return formatRunsData(index.row());
      }

      if (index.column() == Blueprint::Quantity) {
        if (data_.isEmpty())
          gatherAggregationData();

        return data_[index.row()].quantity;
      }
    }
  } else if (role == Qt::UserRole) {
    if (this->hasChildren(index)) {
      if (index.column() == Blueprint::Runs) {
        if (data_.isEmpty())
          gatherAggregationData();

        return data_[index.row()].runs;
      }

      if (index.column() == Blueprint::Quantity) {
        if (data_.isEmpty())
          gatherAggregationData();

        return data_[index.row()].quantity;
      }
    }
  }

  return QIdentityProxyModel::data(index, role);
}

void BlueprintsAggregationProxy::clearCache()
{
  beginResetModel();
  qDebug() << "Aggregation clear cache";
  data_.clear();

  totalQuantity_ = -1;
  totalRuns_ = -1;

  endResetModel();
}

QVariant BlueprintsAggregationProxy::formatRunsData(int row) const
{
  const AggregationInfo& info = data_[row];

  if (info.hasOriginal) {
    if (info.runs <= 0)
      return QChar(0x221E);

    return QString("%1+%2").arg(info.runs).arg(QChar(0x221E));
  }

  return info.runs;
}

void BlueprintsAggregationProxy::calcStats() const
{
  totalRuns_ = 0;
  totalQuantity_ = 0;

  if (data_.isEmpty())
    gatherAggregationData();

  for (int i = 0; i < data_.size(); i++) {
    totalQuantity_ += data_[i].quantity;
    totalRuns_ += data_[i].runs;
  }
}

long BlueprintsAggregationProxy::totatQuantity() const
{
  if (totalQuantity_ < 0)
    calcStats();

  return totalQuantity_;
}

long BlueprintsAggregationProxy::totalRuns() const
{
  if (totalRuns_ < 0)
    calcStats();

  return totalRuns_;
}

void BlueprintsAggregationProxy::gatherAggregationData() const
{
  const QAbstractItemModel* model = this;
  int rows = rowCount();

  data_.clear();
  data_.reserve(rows);

  for (int i = 0; i < rows; i++) {
    QModelIndex index = this->index(i, 0);
    int childNum = model->rowCount(index);
    AggregationInfo info;

    for (int j = 0; j < childNum; j++) {
      QModelIndex runsI = model->index(j, Blueprint::Runs, index);
      QModelIndex quantityI = model->index(j, Blueprint::Quantity, index);

      int quantity = quantityI.data(Qt::UserRole).toInt();
      int runs = runsI.data(Qt::UserRole).toInt();

      info.quantity += quantity;

      if (runs < 0)
        info.hasOriginal = true;
      else
        info.runs += runs * quantity;
    }

    data_.push_back(info);
  }
}
