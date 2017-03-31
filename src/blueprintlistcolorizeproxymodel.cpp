#include "blueprintlistcolorizeproxymodel.h"
#include "blueprintlist.h"

#include <algorithm>

#include <QPainter>
#include <QPair>

BlueprintListColorizeProxyModel::BlueprintListColorizeProxyModel(QObject* parent) :
  QIdentityProxyModel(parent)
{
  bpColorsModel_ = new BPListColorModel(this);
  bpColorsModel_->fetchFromDB("BlueprintsColors");

  connect(bpColorsModel_, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this, SLOT(clearCache()));
  connect(bpColorsModel_, SIGNAL(modelReset()), this, SLOT(clearCache()));
}

BlueprintColorizer BlueprintListColorizeProxyModel::findColorizer(const QModelIndex& index) const
{
  return bpColorsModel_->findColorizer(index);
}

BlueprintColorizer BlueprintListColorizeProxyModel::colorizer(const QModelIndex& index) const
{
  const bool isHeader = !index.parent().isValid();
  QPair<int, int> pos;
  if (isHeader)
    pos = {index.row(), -1};
  else
    pos = {index.parent().row(), index.row()};

  if (!bpColorsCache_.contains(pos))
    bpColorsCache_.insert(pos, findColorizer(index));

  return bpColorsCache_[pos];
}

void BlueprintListColorizeProxyModel::clearCache()
{
  beginResetModel();
  qDebug() << "Reset cache";
  bpColorsCache_.clear();
  endResetModel();
}

void BlueprintListColorizeProxyModel::clearCache(const QModelIndex& tl, const QModelIndex& br)
{
  int begin = tl.row();
  int end = br.row();

  // TODO Implement cache clearing
}

BPListColorModel* BlueprintListColorizeProxyModel::bpColors() const
{
  return bpColorsModel_;
}

void BlueprintListColorizeProxyModel::setBpColors(BPListColorModel* bpColors)
{
  bpColorsModel_ = bpColors;
}

void BlueprintListColorizeProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
  QAbstractItemModel* oldSource = this->sourceModel();
  if (oldSource)
    disconnect(oldSource, 0, this, 0);

  QIdentityProxyModel::setSourceModel(sourceModel);
  clearCache();
}

QVariant BlueprintListColorizeProxyModel::data(const QModelIndex& index, int role) const
{
  const bool isHeader = !index.parent().isValid();
  if (role == Qt::BackgroundRole)
    return colorizer(index).bgColor;
  else if (role == Qt::ForegroundRole)
    return colorizer(index).fgColor;

  return QIdentityProxyModel::data(index, role);
}
