#include "productionmodel.h"

#include "eve/staticdata.h"

ProductionModel::ProductionModel(const Blueprint& bp, QObject* parent)
{
  root_ = new ProductionType(bp);

  connect(root_, SIGNAL(priceUpdated(double)), this, SIGNAL(priceChanged(double)));
  connect(root_, &ProductionType::priceUpdated, [this]() {
    emit dataChanged(index(0, MarketPriceColumn), index(rowCount(), MarketPriceColumn));
  });

  initComponent(root_);
}

QVariant ProductionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole) {
    switch(section) {
      case NameColumn:
        return "Name";
      case MEColumn:
        return "ME";
      case TEColumn:
        return "TE";
      case QuantityColumn:
        return "Quantity";
      case BuildBuyColumn:
        return "Build/Buy";
      case MarketPriceType:
        return "Market Price Type";
      case MarketPriceColumn:
        return "Market Price";
      case BuildPriceColumn:
        return "Build Price";
      case TotalColumn:
        return "Total";
      default:
        return QVariant();
    }
  }

  return QVariant();
}

QModelIndex ProductionModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid())
    return createIndex(row, column, root_);

  ProductionType* node = static_cast<ProductionType*>(parent.internalPointer());
  return createIndex(row, column, node->component(row));
}

QModelIndex ProductionModel::parent(const QModelIndex& index) const
{
  if (!index.isValid())
    return QModelIndex();

  ProductionType* node = static_cast<ProductionType*>(index.internalPointer());
  ProductionType* parent = node->parent();

  if (parent != nullptr)
    return createIndex(node->row(), NameColumn, parent);

  return QModelIndex();
}

int ProductionModel::rowCount(const QModelIndex& parent) const
{
  if (!parent.isValid())
    return 1;

  ProductionType* node = static_cast<ProductionType*>(parent.internalPointer());
  return node->componentsCount();
}

int ProductionModel::columnCount(const QModelIndex& parent) const
{
  return static_cast<int>(ColumnCount);
}

QVariant ProductionModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  ProductionType* node = static_cast<ProductionType*>(index.internalPointer());

  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case NameColumn:
        return eve::SDE.typeName(node->productID());
      case MEColumn:
        if (node->isFinal())
          return "-";

        return node->ME();
      case TEColumn:
        if (node->isFinal())
          return "-";

        return node->TE();
      case QuantityColumn:
        return node->modifiedQuantity();
      case BuildBuyColumn:
        if (node->isFinal())
          return "Buy";

        if (node->build())
          return "Build";
        else
          return "Buy";
      case MarketPriceType:
        return node->priceType();
      case MarketPriceColumn:
        return node->displayData(ProductionType::MarketPrice);
        return QString::number(node->marketPrice(), 'f', 2);
      case BuildPriceColumn:
        if (node->isFinal())
          return "-";

        return node->displayData(ProductionType::BuildPrice);

      case TotalColumn:
        return node->displayData(ProductionType::TotalPrice);

        return QString::number(node->buildPrice(), 'f', 2);
      default:
        return QVariant();
    }
  } else if (role == Qt::TextAlignmentRole) {
    switch (index.column()) {
      case MarketPriceColumn:
      case BuildPriceColumn:
      case TotalColumn:
        return Qt::AlignRight;
      default:
        break;
    }
  } else if (role == Qt::UserRole) {
    switch (index.column()) {
      case MarketPriceType:
        return node->priceType();
      case BuildBuyColumn:
        return node->build();
    }
  }

  return QVariant();
}

bool ProductionModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (data(index, role) != value) {
    ProductionType* node = static_cast<ProductionType*>(index.internalPointer());
    switch (index.column()) {
      case MEColumn:
        node->setME(value.toInt());
        break;
      case TEColumn:
        node->setTE(value.toInt());
        break;
      case MarketPriceType:
        node->setPriceType(value.toInt());
        break;
      case BuildBuyColumn:
        node->setBuild(value.toBool());

        if (node->componentsCount() == 0)
          initComponent(node);

        break;
      default:
        return false;
    }

    emit dataChanged(index, index, QVector<int>() << role);
    return true;
  }
  return false;
}

Qt::ItemFlags ProductionModel::flags(const QModelIndex& index) const
{
  ProductionType* node = static_cast<ProductionType*>(index.internalPointer());

  if (node->isFinal() && (index.column() == BuildBuyColumn))
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (index.column() != NameColumn && index.column() != QuantityColumn && index.column() != BuildPriceColumn)
    return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


bool ProductionModel::hasChildren(const QModelIndex& parent) const
{
  if (!parent.isValid())
    return true;

  ProductionType* node = static_cast<ProductionType*>(parent.internalPointer());

  return !node->isFinal();
}

void ProductionModel::fetchMore(const QModelIndex& parent)
{
  if (!parent.isValid())
    return;

  ProductionType* node = static_cast<ProductionType*>(parent.internalPointer());

  initComponent(node);

  for (ProductionType* p : node->components_) {
    connect(p, &ProductionType::priceUpdated, [this, parent, p](double) {
      QModelIndex i = index(0, 0, QModelIndex());
      emit dataChanged(i, i);
    });

    connect(p, &ProductionType::priceUpdated, [this, parent, p](double) {
      QModelIndex i = index(0, 0, QModelIndex());
      emit dataChanged(i, i);
    });
  }

  beginInsertRows(parent, 0, node->componentsCount() - 1);
  endInsertRows();
}

bool ProductionModel::canFetchMore(const QModelIndex& parent) const
{
  if (!parent.isValid())
    return false;

  ProductionType* node = static_cast<ProductionType*>(parent.internalPointer());

  return !node->isFinal() && !node->componentsCount();
}

ProductionType* ProductionModel::product() const
{
  return root_;
}

void ProductionModel::initComponent(ProductionType* node) const
{
  eve::SDE.initProductionType(node);
}

void ProductionModel::refreshPrice(const QModelIndex& tl, const QModelIndex& br, QVector<int>)
{
  if (tl.column() != MarketPriceColumn)
    return;

  if (!tl.parent().isValid())
    return;

  ProductionType* node = static_cast<ProductionType*>(tl.parent().internalPointer());
//FIXME
}
