#include "assettree.h"

#include <QDebug>
#include <QXmlStreamReader>
#include <QtSql>
#include <QtConcurrent>

#include "eve/staticdata.h"

#include <functional>

AssetTree::AssetTree(QObject* parent)
  : QAbstractItemModel(parent)
{
  hangarFlags[4] = "Division 1";
  hangarFlags[62] = "Deliveries";
  hangarFlags[90] = "Ship Hangar";
  hangarFlags[116] = "Division 2";
  hangarFlags[117] = "Division 3";
  hangarFlags[118] = "Division 4";
  hangarFlags[119] = "Division 5";
  hangarFlags[120] = "Division 6";
  hangarFlags[121] = "Division 7";
  hangarFlags[133] = "Fuel Bay";
  hangarFlags[134] = "Ore Hold";
  hangarFlags[135] = "Gas Hold";
  hangarFlags[136] = "Mineral Hold";
  hangarFlags[155] = "Fleet Hangar";
}

AssetTree::~AssetTree()
{
  clearNodes();
}

QList<QPair<int, qlonglong>> AssetTree::getSelectedIDs() const
{
  QList<QPair<int, qlonglong>> selection;

  for (AssetNode* node : locations)
    selection.append(getChildNodeSelectedIDs(node));

  return selection;
}

void AssetTree::clearNodes()
{
  if (!locations.isEmpty()) {
    for (AssetNode* loc : locations) {
      loc->deleteChilds();
      delete loc;
    }
    locations.clear();
  }
}

void AssetTree::loadFromMemory()
{
  beginResetModel();

  clearNodes();

  eve::ApiKeyList* keys = eve::API.keys();

  for (eve::ApiKeyInfo* key : *keys) {
    AssetNode* root = loadAssetTree(key);
    if (root)
      locations.append(root);
  }

  endResetModel();

  return;
}

AssetTree::AssetNode* AssetTree::loadAssetTree(eve::ApiKeyInfo* key)
{
  QSqlDatabase db = QSqlDatabase::database(":memory:");
  QSqlQuery q(db);


  QString querystr = QString("SELECT * FROM Assets WHERE keyID=%1").arg(key->ID());
  if (!q.exec(querystr))
    qDebug() << "Loading asset list from memory error:" << q.lastError();

  QMap<int, AssetNode*> nodes;
  QMap<int, QVector<AssetNode*>> nodeByFlag;
  QMap<qlonglong, QVector<AssetNode*>> nodeByLoc;

  nodes[0] = new AssetNode;
  nodes[0]->keyID = key->ID();

  while (q.next()) {
    AssetNode* node = new AssetNode;
    node->ID = q.value("ID").toInt();
    node->itemID = q.value("itemID").toLongLong();
    node->typeID = q.value("typeID").toInt();
    node->location = q.value("locationID").toLongLong();
    if (node->location > 0)
      nodeByLoc[node->location].append(node);
    node->quantity = q.value("quantity").toInt();
    node->flag = q.value("flag").toInt();
    node->displayName = eve::SDE.typeName(node->typeID);
    node->keyID = key->ID();

    //Used to eleminate bug: sometimes in server response office has division1 flag
    const int officeID = 27;
    if (node->typeID == officeID)
      node->flag = -1;
    nodeByFlag[node->flag].append(node);

    nodes.insert(node->ID, node);
    node->setParent(nodes.value(q.value("parentID").toInt()));
  }

  if (nodes.size() <= 1)
    return nullptr;

  AssetNode* root = nodes[0];
  QList<qlonglong> locations = nodeByLoc.keys();

  root->childs.clear();
  for (int location : locations) {
    QVector<AssetNode*> nodes = nodeByLoc[location];

    AssetNode* locNode = new AssetNode;
    locNode->location = location;
    locNode->keyID = root->keyID;
    locNode->displayName = eve::SDE.locationName(location);

    for (AssetNode* node : nodes) {
      node->setParent(locNode);
      node->location = -1;
    }

    locNode->setParent(root);
  }

  for (AssetNode* locRoot : root->childs) {
    if (key->type() == "corp")
      insertNodeByFlag(locRoot);
    else {
      for (AssetNode* child : locRoot->childs)
        insertNodeByFlag(child);
    }
  }

  if (key->type() == "char")
    root->displayName = key->charName();
  else
    root->displayName = key->corpName();

  querystr = QString("SELECT locationID FROM BlueprintsFilter WHERE keyID=%1").arg(key->ID());
  if (!q.exec(querystr))
    qDebug() << q.lastError();

  QSet<qlonglong> activeLocations;
  while(q.next())
    activeLocations.insert(q.value("locationID").toLongLong());

  setTreeCheckState(root, activeLocations);

  return root;
}

void AssetTree::setTreeCheckState(AssetTree::AssetNode* root, const QSet<qlonglong>& activeLocations)
{
  bool hasChecked = false;
  bool hasUnchecked = false;

  for (AssetNode* child : root->childs) {
    if (activeLocations.contains(child->location)) {
      child->checkAll();
      hasChecked = true;
    } else {
      setTreeCheckState(child, activeLocations);
      if (child->checked != Qt::Checked)
        hasUnchecked = true;
    }
  }

  if (hasChecked && hasUnchecked)
    root->checked = Qt::PartiallyChecked;
  else if (hasChecked)
    root->checked = Qt::Checked;
  else
    root->checked = Qt::Unchecked;
}

QList<QPair<int, qlonglong>> AssetTree::getChildNodeSelectedIDs(AssetNode* parent) const
{
  QList<QPair<int, qlonglong>> selection;

  if (parent->checked == Qt::Checked) {
    if (parent->location > 0)
      selection.append({parent->keyID, parent->location});
    else if(parent->itemID > 0)
      selection.append({parent->keyID, parent->itemID});
  } else if (parent->checked == Qt::Unchecked)
    return selection;

  for (AssetNode* child : parent->childs) {
    if (!child->isCheckable())
      continue;

    selection.append(getChildNodeSelectedIDs(child));
  }

  return selection;
}

void AssetTree::insertNodeByFlag(AssetTree::AssetNode* locRoot)
{
  if (!locRoot->childs.isEmpty()) {
    for (AssetNode* childRoot : locRoot->childs)
      insertNodeByFlag(childRoot);
  }

  QMap<int, QVector<AssetNode*>> nodeByFlag;
  for (AssetNode* childNode : locRoot->childs)
    nodeByFlag[childNode->flag].append(childNode);

  QList<int> flags = nodeByFlag.keys();
  locRoot->childs.clear();

  for (int flag : flags) {
    if (!hangarFlags.contains(flag)) {
      for (AssetNode* node : nodeByFlag[flag])
        node->setParent(locRoot);

      continue;
    }

    AssetNode* hangarNode = new AssetNode;
    hangarNode->setParent(locRoot);
    hangarNode->displayName = hangarFlags[flag];

    for (AssetNode* node : nodeByFlag[flag])
      node->setParent(hangarNode);
  }

}

int AssetTree::findRow(const AssetTree::AssetNode* node) const
{
  const NodeList& childs = node->parent ? node->parent->childs : locations;
  NodeList::const_iterator pos = qFind(childs, node);
  int position = std::distance(childs.begin(), pos);
  return position;
}



void AssetTree::AssetNode::deleteChilds()
{
  for (AssetNode* child : childs) {
    child->deleteChilds();
    delete child;
  }

  childs.clear();
}

void AssetTree::AssetNode::setParent(AssetTree::AssetNode* parentNode)
{
  parentNode->childs.push_back(this);
  parent = parentNode;
}

void AssetTree::AssetNode::updateCheckState()
{
  bool hasChecked = false;
  bool hasUnchecked = false;
  bool hasPartiallyChecked = false;

  if (childs.empty())
    return;

  for (AssetNode* node : childs) {
    if (node->childs.isEmpty())
      continue;
    if (node->checked == Qt::Checked)
      hasChecked = true;
    else if (node->checked == Qt::Unchecked)
      hasUnchecked = true;
    else
      hasPartiallyChecked = true;

    if (hasPartiallyChecked || (hasChecked && hasUnchecked))
      break;
  }

  if (hasPartiallyChecked || (hasChecked && hasUnchecked))
    checked = Qt::PartiallyChecked;
  else if (hasChecked)
    checked = Qt::Checked;
  else
    checked = Qt::Unchecked;
}

void AssetTree::AssetNode::checkAll()
{
  checked = Qt::Checked;
  for (AssetNode* node : childs)
    node->checkAll();
}

void AssetTree::AssetNode::uncheckAll()
{
  checked = Qt::Unchecked;
  for (AssetNode* node : childs)
    node->uncheckAll();
}

bool AssetTree::AssetNode::isCheckable()
{
  const int labGroup = 413;
  return !childs.isEmpty() ||
         eve::SDE.typeInfo(typeID).groupID == labGroup;

}


QModelIndex AssetTree::index(int row, int column, const QModelIndex& parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid())
    return createIndex(row, column, locations[row]);

  AssetNode* node = static_cast<AssetNode*>(parent.internalPointer());
  return createIndex(row, column, node->childs[row]);
}

QModelIndex AssetTree::parent(const QModelIndex& child) const
{
  if (!child.isValid())
    return QModelIndex();

  AssetNode* node = static_cast<AssetNode*>(child.internalPointer());
  AssetNode* parent = node->parent;
  if (node->parent != nullptr)
    return createIndex(findRow(node), RamificationColumn, parent);

  return QModelIndex();
}

int AssetTree::rowCount(const QModelIndex& parent) const
{
  if (!parent.isValid())
    return locations.size();

  AssetNode* node = static_cast<AssetNode*>(parent.internalPointer());
  return node->childs.size();
}

int AssetTree::columnCount(const QModelIndex& parent) const
{
  return static_cast<int>(ColumnCount);
}

bool AssetTree::hasChildren(const QModelIndex& parent) const
{
  if (!parent.isValid())
    return QAbstractItemModel::hasChildren(parent);

  AssetNode* node = static_cast<AssetNode*>(parent.internalPointer());
  return !node->childs.isEmpty();
}

QVariant AssetTree::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  AssetNode* node = static_cast<AssetNode*>(index.internalPointer());

  switch(index.column()) {
    case NameColumn:
      return nameColumnData(node, role);
    case QuantityColumn:
      return quantityColumnData(node, role);
    case ItemID:
      return itemIDColumnData(node, role);
    default:
      break;
  }

  return QVariant();
}

QVariant AssetTree::nameColumnData(AssetTree::AssetNode* node, int role) const
{
  if (role == Qt::DisplayRole) {
    if (!node->displayName.isEmpty())
      return node->displayName;
    else
      return node->location;
  } else if (role == Qt::CheckStateRole) {
    if (node->isCheckable())
      return node->checked;

    return QVariant();
  }

  return QVariant();
}

QVariant AssetTree::quantityColumnData(AssetTree::AssetNode* node, int role) const
{
  if (role == Qt::DisplayRole) {
    if (node->quantity != 0)
      return node->quantity;
    else
      return QVariant();
  }

  return QVariant();
}

QVariant AssetTree::itemIDColumnData(AssetTree::AssetNode* node, int role) const
{
  if (role == Qt::DisplayRole)
    return node->itemID;

  return QVariant();
}

Qt::ItemFlags AssetTree::flags(const QModelIndex& index) const
{
  if (!index.isValid())
    return 0;

  AssetTree::AssetNode* node = static_cast<AssetTree::AssetNode*>(index.internalPointer());

  switch (index.column()) {
    case RamificationColumn:
      if (node->isCheckable())
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
    default:
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }
}


void AssetTree::updateParentNodeCheckState(AssetNode* node)
{
  if (node->parent) {
    node->parent->updateCheckState();
    QModelIndex parent = createIndex(findRow(node->parent), 0, node->parent);
    emit dataChanged(parent, parent, {Qt::CheckStateRole});

    updateParentNodeCheckState(node->parent);
  }
}

void AssetTree::updateChildNodesCheckState(AssetTree::AssetNode* node)
{
  if (node->childs.isEmpty())
    return;

  QModelIndex first = createIndex(findRow(node->childs.first()), 0, node->childs.first());
  QModelIndex last = createIndex(findRow(node->childs.last()), 0, node->childs.last());
  emit dataChanged(first, last, {Qt::CheckStateRole});

  for (AssetNode* childnode : node->childs)
    updateChildNodesCheckState(childnode);
}

bool AssetTree::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::CheckStateRole) {
    AssetNode* node = static_cast<AssetNode*>(index.internalPointer());

    if (value == Qt::Checked)
      node->checkAll();
    else
      node->uncheckAll();

    updateParentNodeCheckState(node);
    updateChildNodesCheckState(node);

    changed_ = true;
    qDebug() << "Assets filter changed";

    emit selectionChanged();

    return true;
  } else
    return QAbstractItemModel::setData(index, value, role);
}

bool AssetTree::isChanged() const
{
  return changed_;
}

void AssetTree::saveSelection()
{
  QList<QPair<int, qlonglong>> selection = getSelectedIDs();

  QVariantList keyIDs;
  QVariantList locIDs;

  for (auto& pair : selection) {
    keyIDs << pair.first;
    locIDs << pair.second;
  }

  QSqlDatabase db = QSqlDatabase::database(":memory:");
  db.transaction();
  QSqlQuery q(db);

  q.exec("DELETE FROM BlueprintsFilter");

  q.prepare(QString("INSERT OR IGNORE INTO BlueprintsFilter VALUES (?, ?)"));
  q.addBindValue(locIDs);
  q.addBindValue(keyIDs);

  if (!q.execBatch())
    qDebug() << "Filter insertion error -" << q.lastError();

  if (!db.commit()) {
    qDebug() << db.lastError();
    db.rollback();
  }

  changed_ = false;
}

void AssetTree::queueFetch()
{
  fetched = false;
}

QVariant AssetTree::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole) {
    switch (section) {
      case NameColumn:
        return "Name";
      case QuantityColumn:
        return "Quantity";
      case ItemID:
        return "ItemID";
    }
  }

  return QVariant();
}
