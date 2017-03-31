#include "blueprintlist.h"

#include <QXmlStreamReader>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include <QtConcurrent>

#include <algorithm>
#include <functional>

#include "eve/staticdata.h"

BlueprintList::BlueprintList(QObject* parent) :
  QAbstractItemModel(parent)
{
  tableBpColumns = QMap<int, QString> {
    { Blueprint::Name, "bp.typeName AS typeName, bp.typeID AS typeID" },
    { Blueprint::Runs, "bp.runs AS runs" },
    { Blueprint::MEc, "bp.ME AS ME" },
    { Blueprint::TEc, "bp.TE AS TE" },
    { Blueprint::LocationIDc, "bp.locationID AS locationID" },
    { Blueprint::Owner, "bp.keyID AS ownerID" },
    { Blueprint::Activity, "ij.activityID AS activityID"}
  };

  tableBpGroups = QMap<int, QString> {
    { Blueprint::Name, "bp.typeName , bp.typeID " },
    { Blueprint::Runs, "bp.runs " },
    { Blueprint::MEc, "bp.ME " },
    { Blueprint::TEc, "bp.TE " },
    { Blueprint::LocationIDc, "bp.locationID " },
    { Blueprint::Owner, "bp.keyID " },
    { Blueprint::Activity, "ij.activityID "}
  };

  tableUnionColumns_ = QMap<int, QString> {
    { Blueprint::Name, "it.typeName as typeName, it.typeID as typeID" },
    { Blueprint::Runs, "0 as runs" },
    { Blueprint::MEc, "0 as ME" },
    { Blueprint::TEc, "0 as TE" },
    { Blueprint::LocationIDc, "0 as locationID" },
    { Blueprint::Owner, "-1 AS ownerID" },
    { Blueprint::Activity, "0 as activityID"}
  };

  groupColumns_ = QSet<int> {
    Blueprint::Name,
    Blueprint::Runs,
    Blueprint::MEc,
    Blueprint::TEc,
    Blueprint::Activity,
    Blueprint::LocationIDc,
    Blueprint::Owner
  };
}

void BlueprintList::loadFromMemory()
{
  QSqlDatabase db = QSqlDatabase::database(":memory:");
  if (!db.isOpen()) {
    db.setDatabaseName("temp.sqlite");
    db.open();
  }

  QSqlQuery q(db);

  if (showAll_)
    q.exec(QString("ATTACH DATABASE \"%1\" AS sde").arg(eve::SDE.sdeFileLocation()));

  QString querystr = makeFetchQuery(showAll_);

  if (!q.exec(querystr)) {
    qWarning() << q.lastError();
    qWarning() << querystr;
    return;
  }

  QVector<Blueprint> blueprints;
  beginResetModel();

  while (q.next()) {
    Blueprint bp;
    bp.typeName = q.value("typeName").toString();

    if (hasGroupColumn(Blueprint::Runs))
      bp.runs = q.value("runs").toInt();

    if (hasGroupColumn(Blueprint::MEc))
      bp.ME = q.value("ME").toInt();

    if (hasGroupColumn(Blueprint::TEc))
      bp.TE = q.value("TE").toInt();

    if (hasGroupColumn(Blueprint::LocationIDc))
      bp.locationID = q.value("locationID").toLongLong();

    if (hasGroupColumn(Blueprint::Activity))
      bp.activity = q.value("activityID").toInt();

    if (hasGroupColumn(Blueprint::Owner))
      bp.ownerID = q.value("ownerID").toInt();

    bp.quantity = q.value("quantity").toInt();
    bp.typeID = q.value("typeID").toInt();
    bp.prodTypeID = eve::SDE.productTypeIDbyBPTypeID(bp.typeID);
    if (bp.runs <= 0)
      bp.original = true;
    else
      bp.original = false;

    invType typeinfo = eve::SDE.typeInfo(bp.prodTypeID);
    bp.prodGroupID = typeinfo.groupID;
    bp.prodCategoryID = typeinfo.categoryID;
    bp.prodMarketGroups = eve::SDE.marketGroupsModel()->getParentGroups(typeinfo.marketGroup);
    bp.prodMetaLevel = typeinfo.metaLevel;

    blueprints.append(bp);
  }

  std::sort(blueprints.begin(), blueprints.end(), BlueprintComparator);

  if (!headers.isEmpty())
    headers.clear();

  for (int i = 0; i < blueprints.size();) {
    BlueprintHeader bph(blueprints[i]);

    while (!flat_ && (i < blueprints.size() && blueprints[i].typeName == bph.typeName)) {
      const Blueprint& bp = blueprints[i];
      bph.addBlueprint(bp);
      ++i;
    }

    if (flat_) {
      bph.addBlueprint(blueprints[i]);
      bph.runs = blueprints[i].runs;
      bph.activity = blueprints[i].activity;
      bph.prodMetaLevel = blueprints[i].prodMetaLevel;

      bph.blueprints.clear();
      ++i;
    }

    headers.append(bph);
  }

  qDebug() << headers.size();


  if (showAll_)
    q.exec("DETACH sde");

  endResetModel();
  emit modelFetched();

  if (headers.isEmpty())
    return;
}

QString BlueprintList::makeFetchQuery(bool showAll)
{
  QString querystr = QString(
                       "SELECT %1, COUNT(*) quantity "
                       "FROM Blueprints bp "
                       "LEFT JOIN IndustryJobs ij ON "
                       "  bp.itemID = ij.blueprintID "
                       "GROUP BY %2 "
                       "%3 "
                       "ORDER BY bp.typeName, quantity"
                     );

  QStringList selectList = makeSelectList(tableBpColumns);
  QStringList groupList = makeSelectList(tableBpGroups);
  querystr = querystr.arg(selectList.join(',')).arg(groupList.join(','));

  if (showAll) {
    QString unionQuery = QString(
                           "UNION "
                           "SELECT "
                           "  %1, "
                           "  0 as quantity "
                           "FROM "
                           "  sde.industryBlueprints ib "
                           "INNER JOIN sde.invTypes it ON "
                           "  it.typeID=ib.typeID AND "
                           "  it.published=1 "
                           "WHERE ib.typeID in ("
                           "  SELECT "
                           "    ib.typeID "
                           "  FROM "
                           "    sde.industryBlueprints ib "
                           "  EXCEPT"
                           "  SELECT "
                           "    bp.typeID "
                           "  FROM "
                           "    Blueprints bp "
                           ") "
                         ).arg(makeSelectList(tableUnionColumns_).join(','));

    querystr = querystr.arg(unionQuery);
  } else
    querystr = querystr.arg("");

  return querystr;
}

QSet<int> BlueprintList::groupColumns() const
{
  return groupColumns_;
}

void BlueprintList::setGroupColumns(const QSet<int>& groupColumns)
{
  groupColumns_ = groupColumns;
}

void BlueprintList::addGroupColumn(int column)
{
  groupColumns_.insert(column);
}

void BlueprintList::removeGroupColumn(int column)
{
  groupColumns_.remove(column);
}

const Blueprint& BlueprintList::blueprint(int hid, int row) const
{
  return headers[hid].blueprints[row];
}

const Blueprint& BlueprintList::blueprint(const QModelIndex& index) const
{
  if (index.parent().isValid())
    return headers[index.parent().row()].blueprints[index.row()];

  return headers[index.row()];
}

const BlueprintHeader& BlueprintList::blueprintHeader(int hid) const
{
  return headers[hid];
}

QModelIndex BlueprintList::index(int row, int column, const QModelIndex& parent) const
{
  if (parent.isValid())
    return createIndex(row, column, parent.row());


  if (row >= headers.size() || row < 0 || !hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid())
    return createIndex(row, column, (quintptr)std::numeric_limits<quintptr>::max());

  return createIndex(row, column, parent.row());
}

QModelIndex BlueprintList::parent(const QModelIndex& child) const
{
  if (!child.isValid())
    return QModelIndex();


  quintptr pid = child.internalId();
  if (pid == std::numeric_limits<quintptr>::max())
    return QModelIndex();

  return createIndex(pid, 0, (quintptr)std::numeric_limits<quintptr>::max());
}

int BlueprintList::rowCount(const QModelIndex& parent) const
{
  if (!parent.isValid())
    return headers.size();

  return headers[parent.row()].blueprints.size();
}

int BlueprintList::columnCount(const QModelIndex& parent) const
{
  return static_cast<int>(Blueprint::ColumnCount);
}

QVariant BlueprintList::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole)
    return blueprint(index).displayData(index.column());
  else if (role == BlueprintRole) {
    if (index.parent().isValid())
      return QVariant::fromValue(blueprint(index.parent().row(), index.row()));

    return QVariant::fromValue(blueprintHeader(index.row()).toBlueprint());
  } else if (role == BlueprintHeaderRole) {
    if (!index.parent().isValid())
      return QVariant::fromValue(blueprintHeader(index.row()));

    return QVariant();

  } else if (role == RawData)
    return blueprint(index).data(index.column());
  else if (role == CategoryID)
    return blueprint(index).prodCategoryID;
  else if (role == GroupID)
    return blueprint(index).prodGroupID;
  else if (role == IsHeaderRole) {
    if (flat_)
      return false;

    return !index.parent().isValid();
  }

  return QVariant();
}

QVariant BlueprintList::userRoleData(const QModelIndex& index) const
{
  switch(index.column()) {

  }
  return QVariant();

}

QStringList BlueprintList::makeSelectList(const QMap<int, QString>& names) const
{
  QStringList selectList;
  for (int i = 0; i < Blueprint::ColumnCount; i++) {
    if (!names.contains(i))
      continue;

    if (hasGroupColumn(i))
      selectList << names[i];
  }

  return selectList;
}

bool BlueprintList::showAll() const
{
  return showAll_;
}

void BlueprintList::setShowAll(bool showAll)
{
  showAll_ = showAll;
}

bool BlueprintList::isFlat() const
{
  return flat_;
}


void BlueprintList::setFlat(bool flat)
{
  flat_ = flat;
}

bool BlueprintList::hasChildren(const QModelIndex& parent) const
{
  if (!parent.isValid())
    return true;

  if (!parent.parent().isValid()) {
    if (flat_)
      return false;

    return true;
  }

  return false;
}

QVariant BlueprintList::headerData(int section, Qt::Orientation orientation, int role) const
{
  typedef Blueprint::Columns Columns;
  if (role == Qt::DisplayRole) {
    switch (section) {
      case Columns::Name:
        return "Blueprint name";
      case Columns::Activity:
        return "";
      case Columns::Runs:
        return "Runs";
      case Columns::Quantity:
        return "Quantity";
      case Columns::MEc:
        return "ME";
      case Columns::TEc:
        return "TE";
      case Columns::LocationIDc:
        return "Location";
      case Columns::MetaLevel:
        return "Meta level";
      case Columns::Owner:
        return "Owner";
      default:
        break;
    }
  }

  return QVariant();
}

bool BlueprintList::hasGroupColumn(int column) const
{
  return groupColumns_.contains(column);
}
