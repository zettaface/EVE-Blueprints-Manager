#include "bpcolorsmodel.h"

#include <QColor>
#include <QComboBox>
#include <QSortFilterProxyModel>
#include <QtSql>
#include <QListWidget>
#include <QLineEdit>

#include <functional>
#include <sstream>
#include <algorithm>
#include <yaml-cpp/yaml.h>

#include "eve/api.h"
#include "eve/staticdata.h"
#include "blueprintlist.h"
#include "bpfilterfactory.h"
#include "categoryfilterproxymodel.h"

BPListColorModel::BPListColorModel(QObject* parent) :
  QAbstractTableModel(parent)
{
  using FuncID = BlueprintColorizer::Func;
  using ValueID = BlueprintColorizer::ValueType;

  funcNames_.insert(FuncID::LESS, "<");
  funcNames_.insert(FuncID::LESS_EQUAL, "<=");
  funcNames_.insert(FuncID::GREATER, ">");
  funcNames_.insert(FuncID::GREATER_EQUAL, ">=");
  funcNames_.insert(FuncID::EQUAL, "=");

  valNames_.insert(ValueID::Runs, "Runs");
  valNames_.insert(ValueID::Quantity, "Quantity");

  installEventFilter(this);
}

int BPListColorModel::rowCount(const QModelIndex& parent) const
{
  return count();
}

int BPListColorModel::columnCount(const QModelIndex& parent) const
{
  return ColumnCount;
}

QVariant BPListColorModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {
    switch(index.column()) {
      case ForegroundColor:
        return data_[index.row()].fgColor;
      case BackgroundColor:
        return data_[index.row()].bgColor;
      case Filter:
        return data_[index.row()].filter()->displayData().mid(1, data_[index.row()].filter()->displayData().length() - 2);
      case Priority:
        return data_[index.row()].priority;
      default:
        return QVariant();
    }
  } else if (role == Qt::DecorationRole) {
    switch(index.column()) {
      case ForegroundColor:
        return data_[index.row()].fgColor;
      case BackgroundColor:
        return data_[index.row()].bgColor;
      default:
        return QVariant();
    }
  } else if (role == Qt::BackgroundRole) {
    if (index.column() == Filter)
      return data_[index.row()].bgColor;
  } else if (role == Qt::ForegroundRole) {
    if (index.column() == Filter)
      return data_[index.row()].fgColor;
  } else if (role == Qt::UserRole) {
    switch(index.column()) {
      case ForegroundColor:
        return data_[index.row()].fgColor;
      case BackgroundColor:
        return data_[index.row()].bgColor;
      case Filter:
        return QVariant::fromValue<QSharedPointer<BpFilter>>(data_[index.row()].filter());
      case Priority:
        return data_[index.row()].priority;
      default:
        return QVariant();
    }
  }

  return QVariant();
}

QVariant BPListColorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole) {
    if (orientation == Qt::Horizontal) {
      switch(section) {
        case ForegroundColor:
          return "Text Color";
        case BackgroundColor:
          return "Background Color";
        case Filter:
          return "Filter";
        case Priority:
          return "Priority";
        default:
          return QVariant();
      }
    } else
      return section + 1;

  }

  return QAbstractItemModel::headerData(section, orientation, role);
}

bool BPListColorModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (role == Qt::UserRole && index.column() == Filter)
    data_[index.row()].setFilter(value.value<QSharedPointer<BpFilter>>());
  else {

    switch(index.column()) {
      case BackgroundColor:
        data_[index.row()].bgColor = value.value<QColor>();
        emit dataChanged(this->index(index.row(), Filter, index.parent()), this->index(index.row(), Filter, index.parent()), {Qt::BackgroundRole});
        break;
      case ForegroundColor:
        data_[index.row()].fgColor = value.value<QColor>();
        emit dataChanged(this->index(index.row(), Filter, index.parent()), this->index(index.row(), Filter, index.parent()), {Qt::ForegroundRole});
        break;
      case Priority:
        data_[index.row()].priority = value.toInt();
        break;
      default:
        qDebug() << "Wrong column" << index;
        return false;
    }
  }

  emit dataChanged(index, index);
  return true;
}

bool BPListColorModel::insertRows(int row, int count, const QModelIndex& parent)
{
  if (row < 0 || row > rowCount() || count < 1)
    return false;

  beginInsertRows(parent, row, row + count - 1);
  data_.insert(row, count, BlueprintColorizer());
  endInsertRows();
  return true;
}

bool BPListColorModel::removeRows(int row, int count, const QModelIndex& parent)
{
  if (row < 0 || count < 1 || (row + count) > rowCount())
    return false;

  beginRemoveRows(parent, row, row + count - 1);
  data_.remove(row, count);
  endRemoveRows();
  return true;
}

bool BPListColorModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
  beginMoveRows(sourceParent, sourceRow, sourceRow + count, destinationParent, destinationChild);
  qDebug() << "Keep the bodies movin'";
  endMoveRows();
  return false;
}

BlueprintColorizer BPListColorModel::findColorizer(const QModelIndex& index) const
{
  int bcId = -1;
  for (int i = 0; i < data_.size(); i++) {
    if (data_[i].filter()->validate(index)) {
      if (bcId < 0)
        bcId = i;

      if (data_[i].priority > data_[bcId].priority)
        bcId = i;
    }
  }

  if (bcId < 0)
    return BlueprintColorizer();

  return data_[bcId];
}

void BPListColorModel::fetchFromDB(QString dbname)
{
  if (dbname.isEmpty())
    return;

  beginResetModel();
  data_.clear();

  QSqlDatabase db = QSqlDatabase::database(":memory:");
  QSqlQuery q(db);

  if (!q.exec(QString("SELECT * FROM %1").arg(dbname))) {
    qDebug() << q.lastError();
    return;
  }

  while(q.next()) {
    BlueprintColorizer bc;
    bc.fgColor =    q.value("fgColor").value<QColor>();
    bc.bgColor =    q.value("bgColor").value<QColor>();
    bc.priority =   q.value("priority").toInt();

    QByteArray rawFilter = q.value("filter").toByteArray();

    bc.setFilter(BpFilterFactory::loadFilters(QJsonDocument::fromBinaryData(rawFilter).object()));

    data_.push_back(bc);
  }

  lastUsedDB = dbname;
  endResetModel();
}

void BPListColorModel::save(QIODevice* device)
{
  QDataStream stream(device);

  YAML::Node array;
  YAML::Emitter emitter;
  emitter << YAML::BeginSeq;

  QJsonArray bcs;
  for (const BlueprintColorizer& bc : data_) {
    YAML::Node bcj;

    bcj["bgColor"] = bc.bgColor.name().toStdString();
    bcj["fgColor"] = bc.fgColor.name().toStdString();
    bcj["filter"] = BpFilterFactory::instance().saveFiltersAsYaml(bc.filter());
    bcj["priority"] = bc.priority;

    emitter << YAML::BeginMap;

    emitter << YAML::Key << "bgcolor";
    emitter << YAML::Value << bc.bgColor.name().toStdString();

    emitter << YAML::Key << "fgcolor";
    emitter << YAML::Value << bc.fgColor.name().toStdString();

    emitter << YAML::EndMap;

    array.push_back(bcj);
  }

  std::stringstream ss;
  ss << array;

  qDebug() << QString::fromStdString(ss.str());
  device->write(ss.str().data());
}

bool BPListColorModel::load(QIODevice* device)
{
  QDataStream stream(device);
  QByteArray data = device->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);

  QJsonArray bcsj = doc.array();

  beginResetModel();
  data_.clear();

  for (int i = 0; i < bcsj.size(); i++) {
    QJsonObject bcj = bcsj[i].toObject();
    BlueprintColorizer bc;

    bc.fgColor = QColor(bcj["fgcolor"].toString());
    bc.bgColor = QColor(bcj["bgcolor"].toString());
    bc.priority = bcj["priority"].toInt();

    bc.setFilter(BpFilterFactory::instance().loadFilters(bcj["filter"].toObject()));
  }

  endResetModel();

  return true;
}

void BPListColorModel::submitToDB() const
{
  submitToDB("BlueprintsColors");
}

void BPListColorModel::submitToDB(QString dbname) const
{
  qDebug() << dbname;
  if (dbname.isEmpty()) {
    qDebug() << "Empty db name. Submit to nowhere";
    return;
  }

  QSqlDatabase db = QSqlDatabase::database(":memory:");
  db.transaction();

  QSqlQuery q(db);
  q.exec(QString("DELETE FROM %1").arg(dbname));
  q.prepare(QString("INSERT INTO %1 VALUES (?, ?, ?, ?)").arg(dbname));

  QVariantList fgColors;
  QVariantList bgColors;
  QVariantList filters;
  QVariantList priorities;

  for (BlueprintColorizer color : data_) {
    QJsonDocument json (BpFilterFactory::saveFiltersAsJson(color.filter()));
    fgColors << color.fgColor;
    bgColors << color.bgColor;

    filters <<  json.toBinaryData();
    priorities << color.priority;
  }

  q.addBindValue(fgColors);
  q.addBindValue(bgColors);
  q.addBindValue(filters);
  q.addBindValue(priorities);

  if (!q.execBatch())
    qDebug() << q.lastError() << dbname << fgColors.size() << bgColors.size() << filters.size() << priorities.size();

  if (!db.commit()) {
    qDebug() << db.lastError();
    db.rollback();
  }
}

int BPListColorModel::count() const
{
  return data_.size();
}

void BPListColorModel::fetchFromDB()
{
  fetchFromDB("BlueprintsColors");
}

Qt::ItemFlags BPListColorModel::flags(const QModelIndex& index) const
{
  if (index.column() != BackgroundColor && index.column() != ForegroundColor)
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

  return QAbstractTableModel::flags(index) | Qt::ItemIsDragEnabled;
}


Qt::DropActions BPListColorModel::supportedDropActions() const
{
  return Qt::MoveAction | Qt::CopyAction;
}

Qt::DropActions BPListColorModel::supportedDragActions() const
{
  return QAbstractItemModel::supportedDragActions();
}


QMap<int, QVariant> BPListColorModel::itemData(const QModelIndex& index) const
{
  return QMap<int, QVariant> {
    {Qt::UserRole, index.data(Qt::UserRole)}
  };
  return QAbstractItemModel::itemData(index);
}

bool BPListColorModel::setItemData(const QModelIndex& index, const QMap<int, QVariant>& roles)
{
  if (!roles.contains(Qt::UserRole))
    return false;

  setData(index, roles[Qt::UserRole], Qt::UserRole);
  return true;
  return QAbstractItemModel::setItemData(index, roles);
}


QStringList BPListColorModel::mimeTypes() const
{
  QStringList types;
  types << "application/vnd.text.list";
  return types;
}

QMimeData* BPListColorModel::mimeData(const QModelIndexList& indexes) const
{
  QMimeData* mimeData = new QMimeData();
  QByteArray encodedData;

  QDataStream stream(&encodedData, QIODevice::WriteOnly);

  for (const QModelIndex& index : indexes) {
    if (index.isValid() && index.column() == 0)
      stream << data_[index.row()];
  }

  mimeData->setData("application/vnd.text.list", encodedData);
  return mimeData;
}

bool BPListColorModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
  if (action == Qt::IgnoreAction)
    return true;

  int beginRow;
  if (row != -1)
    beginRow = row;
  else if (parent.isValid())
    beginRow = parent.row();
  else
    beginRow = rowCount();

  QByteArray encodedData = data->data("application/vnd.text.list");
  QDataStream stream(&encodedData, QIODevice::ReadOnly);

  QVector<BlueprintColorizer> newItems;

  int rows = 0;

  while (!stream.atEnd()) {
    BlueprintColorizer bc ;
    stream >> bc;
    newItems.push_back(bc);
    ++rows;
  }

  if (action == Qt::MoveAction) {
    beginInsertRows(QModelIndex(), beginRow, beginRow + rows - 1);
    data_.insert(beginRow, rows, BlueprintColorizer());

    for (BlueprintColorizer& bc : newItems) {
      data_[beginRow] = bc;
      ++beginRow;
    }

    endInsertRows();
  } else if (action == Qt::CopyAction)
    qDebug() << "Copy";

  return true;
}
