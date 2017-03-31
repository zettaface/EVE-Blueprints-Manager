#include "oretablemodel.h"

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

#include "eve/api.h"
#include "eve/staticdata.h"
#include "priceinfo.h"

int OreTableModel::rowCount(const QModelIndex& parent) const
{
  return data_.size();
}

int OreTableModel::columnCount(const QModelIndex& parent) const
{
  return Columns::ColumnsCount;
}

QVariant OreTableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case Name:
        return data_[index.row()].name;
      case Price:
        return  PriceInfo::formatPrice(data_[index.row()].price);
      case Tritanium:
      case Pyerite:
      case Mexallon:
      case Isogen:
      case Nocxium:
      case Zydrine:
      case Megacyte:
        return mineralDataColumn(index);
      default:
        break;
    }
  } else if (role == Qt::CheckStateRole) {
    if (index.column() == Name) {
      if (data_[index.row()].checked)
        return QVariant::fromValue<Qt::CheckState>(Qt::Checked);

      return QVariant::fromValue<Qt::CheckState>(Qt::Unchecked);
    }
  } else if (role == Qt::DecorationRole) {
    if (index.column() == Name)
      return data_[index.row()].icon;
  }

  return QVariant();
}

void OreTableModel::loadFromMemory()
{
  QSqlDatabase db = eve::SDE.getConnection();

  QString query = "SELECT "
                  "  it.typeID, "
                  "  it.basePrice, "
                  "  skills.valueInt as skillID "
                  "FROM invTypes it "
                  "INNER JOIN invGroups ig ON "
                  "  it.groupID = ig.groupID "
                  "INNER JOIN dgmTypeAttributes skills ON "
                  "  it.typeID = skills.typeID "
                  "WHERE "
                  "  ig.categoryID = 25 AND "
                  "  it.marketGroupID < 1000 AND "
                  "  it.typeName LIKE 'Compressed %' AND "
                  "  skills.attributeID=790 "
                  "ORDER BY "
                  "  ig.groupID ASC, "
                  "  it.basePrice ASC";

  QSqlQuery q(db);

  if (!q.exec(query)) {
    qWarning() << q.lastError();
    return;
  }

  beginResetModel();

  while (q.next()) {
    ItemRefinery ir;

    qlonglong id = q.value("typeID").toLongLong();
    invType it = eve::SDE.typeInfo(id);

    ir.name = it.typeName;
    ir.price = q.value("basePrice").toDouble();
    ir.skillID = q.value("skillID").toInt();
    ir.icon = eve::SDE.icon(it).scaled(32, 32);
    ir.minerals = refineInfoToMinerals(eve::SDE.refineInfo(it));
    ir.priceInfo = eve::API.priceInfo(id);

    data_.push_back(ir);
  }

  endResetModel();
}

QVector<double> OreTableModel::refineInfoToMinerals(const RefineInfo& refineInfo) const
{
  QVector<double> minerals (7, 0);

  for (const QPair<invType, int>& p : refineInfo.materials) {
    if (p.first.ID < 34 || p.first.ID > 40)
      continue;

    minerals[p.first.ID - 34] = p.second;
  }

  return minerals;
}

QVariant OreTableModel::mineralDataColumn(const QModelIndex& index) const
{
  int q = data_[index.row()].minerals[index.column() - static_cast<int>(Tritanium)];
  if (q == 0)
    return QVariant();

  return q;
}


Qt::ItemFlags OreTableModel::flags(const QModelIndex& index) const
{
  if (index.column() == Name)
    return Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant OreTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole) {
    if (orientation == Qt::Horizontal) {
      switch (section) {
        case Name:
          return "Name";
        case Price:
          return "Price";
        case Tritanium:
          return "Tritanium";
        case Pyerite:
          return "Pyerite";
        case Mexallon:
          return "Mexallon";
        case Isogen:
          return "Isogen";
        case Nocxium:
          return "Nocxium";
        case Zydrine:
          return "Zydrine";
        case Megacyte:
          return "Megacyte";
        default:
          return QVariant();
      }
    }
  }

  return QVariant();
}

QVector<ItemRefinery> OreTableModel::selectedItems() const
{
  QVector<ItemRefinery> selection;

  for (const ItemRefinery& ir : data_) {
    if (ir.checked)
      selection.push_back(ir);
  }

  return selection;
}


bool OreTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (role == Qt::CheckStateRole) {
    if (value == Qt::Checked)
      data_[index.row()].checked = true;
    else
      data_[index.row()].checked = false;
    emit dataChanged(this->index(index.row(), index.column()), this->index(index.row(), index.column()), {role});
  } else
    QAbstractItemModel::setData(index, value, role);
}
