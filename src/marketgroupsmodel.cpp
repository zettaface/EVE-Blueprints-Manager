#include "marketgroupsmodel.h"

#include <QtSql>
#include <QDebug>

#include "eve/staticdata.h"

void MarketGroupsModel::fetchFromSDE()
{
  QSqlDatabase db = eve::StaticData::getConnection();
  if (!db.isOpen()) {
    qDebug() << db.lastError();
    return;
  }

  //TODO: Select only necessary (ex. root groups 4,9,11,157,475,477,955)
  QString querystr = QString("SELECT * FROM invMarketGroups");
  QSqlQuery query(db);
  if (!query.exec(querystr)) {
    qDebug() << query.lastError();
    return;
  }

  _tree.clear();

  MarketGroupNode root;
  _tree[0] = root;
  _tree.insert(0, root);
  root.name = "root";
  root.marketGroupID = 0;

  MarketGroupNode allGroups;
  allGroups.marketGroupID = -1;
  allGroups.name = "All";
  allGroups.parentGroup = 0;

  _tree.insert(-1, allGroups);

  while (query.next()) {
    MarketGroupNode node;
    node.marketGroupID = query.value("marketGroupID").toInt();
    node.name = query.value("marketGroupName").toString();
    node.description = query.value("description").toString();
    node.parentGroup = query.value("parentGroupID").isNull() ? 0 : query.value("parentGroupID").toInt();

    _tree.insert(node.marketGroupID, node);
  }

  // Make parent-child relations
  for (MarketGroupNode& node : _tree.values()) {
    if (node.marketGroupID == 0)
      continue;

    _tree[node.parentGroup].childs.push_back(node.marketGroupID);
  }
}


QModelIndex MarketGroupsModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid())
    return createIndex(row, column, _tree[0].childs[row]); // data at root->row

  const MarketGroupNode& parentNode = _tree[parent.internalId()];
  return createIndex(row, column, parentNode.childs[row]);
}

QModelIndex MarketGroupsModel::index(int groupID, int column) const
{
  if (!_tree.contains(groupID))
    return QModelIndex();

  return createIndex(findRow(_tree[groupID]), column, groupID);
}

QModelIndex MarketGroupsModel::parent(const QModelIndex& child) const
{
  if (!child.isValid())
    return QModelIndex();

  const MarketGroupNode& node = _tree[child.internalId()];

  if (node.parentGroup)
    return createIndex(findRow(_tree[node.parentGroup]), Ramification, node.parentGroup);

  return QModelIndex();
}

int MarketGroupsModel::rowCount(const QModelIndex& parent) const
{
  if (_tree.isEmpty())
    return 0;

  if (!parent.isValid())
    return _tree[0].childs.size();

  const MarketGroupNode& node = _tree[parent.internalId()];
  return node.childs.size();
}

int MarketGroupsModel::columnCount(const QModelIndex& parent) const
{
  return static_cast<int>(ColumnCount);
}

QVariant MarketGroupsModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  const MarketGroupNode& node = _tree[index.internalId()];
  switch(role) {
    case Qt::DisplayRole:
      return displayRoleData(index);
    case Qt::ToolTipRole:
      return node.description;
    case Qt::UserRole:
      return userData(index);
    case Qt::UserRole+1: //TODO: Fix this
      return node.marketGroupID;
    default:
      return QVariant();
  }
}

QVariant MarketGroupsModel::data(int groupID, int column, int role) const
{
  return data(index(groupID, column), role);
}


bool MarketGroupsModel::hasChildren(const QModelIndex& parent) const
{
  if (!parent.isValid())
    return QAbstractItemModel::hasChildren(parent);

  const MarketGroupNode& node = _tree[parent.internalId()];
  return !node.childs.isEmpty();
}

int MarketGroupsModel::parentsCount(int index) const
{
  int depth = 0;

  if (!_tree.contains(index))
    return depth;

  while (index) {
    ++depth;
    index = _tree[index].parentGroup;
  }

  return depth;
}

QSet<int> MarketGroupsModel::getParentGroups(int index) const
{
  QSet<int> groups;

  if (!_tree.contains(index))
    return groups;

  while (index) {
    groups.insert(index);
    index = _tree[index].parentGroup;
  }

  groups.insert(-1);

  return groups;
}

int MarketGroupsModel::groupDepth(int groupID) const
{
  if (groupID <= 0)
    return 0;

  if (!_tree.contains(groupID))
    return 1000;

  int currentDepth = 1;

  while (groupID) {
    currentDepth++;
    groupID = _tree[groupID].parentGroup;
  }

  return currentDepth;
}

int MarketGroupsModel::findRow(const MarketGroupNode& node) const
{
  const NodeList& childs = _tree[node.parentGroup].childs;
  NodeList::const_iterator pos = qFind(childs, node.marketGroupID);
  int position = std::distance(childs.begin(), pos);
  return position;
}

QString MarketGroupsModel::groupPath(const MarketGroupNode& node, const QString& currentPath) const
{
  QString path = node.name + currentPath;
  if (node.parentGroup != 0)
    return groupPath(_tree[node.parentGroup], path.prepend('/'));

  return path;
}

QVariant MarketGroupsModel::displayRoleData(const QModelIndex& index) const
{
  const MarketGroupNode& node = _tree[index.internalId()];

  switch(index.column()) {
    case Name:
      return node.name;
    case Description:
      return node.description;
    case GroupID:
      return node.marketGroupID;
    default:
      return QVariant();
  }
}

QVariant MarketGroupsModel::userData(const QModelIndex& index) const
{
  int id = index.internalId();
  if (id < 0)
    return QVariant();

  const MarketGroupNode& node = _tree[id];

  switch(index.column()) {
    case Name:
      return groupPath(node);
    case GroupID:
      return node.marketGroupID;
    default:
      return QVariant();
  }
}
