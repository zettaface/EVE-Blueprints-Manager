#include "staticdata.h"

#include <QFile>
#include <QFileSystemModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>
#include <QVariant>
#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <QStringList>

#include "api.h"

namespace eve {
StaticData SDE;
QString StaticData::dbLocation = "sde.sqlite";

StaticData::StaticData()
{
  bpCategories_ = new BPCategoriesModel;
  bpGroups_ = new BPGroupsModel;

}

StaticData::~StaticData()
{
  bpCategories_->deleteLater();
  bpGroups_->deleteLater();
}

void StaticData::init()
{
  marketGroups = new MarketGroupsModel; //TODO Parent thing
  marketGroups->fetchFromSDE();

  industryActivities = new IndustryActivityTypeModel;
  industryActivities->fetchFromSDE();

  initializeCategoriesModel();
  initializeGroupsModel();
}

QString StaticData::typeName(int typeID)
{
  if (typeID <= 0)
    return "";

  if (!typeNames.contains(typeID))
    loadTypeName(typeID);

  return typeNames[typeID].typeName;
}

void StaticData::loadLocationName(qlonglong locationID)
{
  QSqlDatabase db = getConnection();
  if (!db.isOpen()) {
    locationNames[locationID] = "SDE Error";
    return;
  }

  QSqlQuery query;
  qlonglong loc = locationID;
  QString querystr;

  if (locationID >= 66000000 && locationID < 66014933) {
    //staStations
    loc -= 6000001;
    querystr = QString("SELECT "
                       "  stationName as locName "
                       "FROM "
                       "  staStations "
                       "WHERE "
                       "  stationID=%1").arg(loc);
  } else if (locationID >= 60000000 && locationID < 61000000)
    querystr = QString("SELECT "
                       "  stationName as locName "
                       "FROM "
                       "  staStations "
                       "WHERE "
                       "  stationID=%1").arg(loc);
  else if (locationID >= 66014934 && locationID < 67999999) {
    //conqStations
    loc -= 6000000;
    QString q = QString("SELECT "
                        "  stationName "
                        "FROM "
                        "  ConquarableStations "
                        "WHERE "
                        "  stationID=%1").arg(loc);
    QSqlDatabase db = QSqlDatabase::database(":memory:");
    QSqlQuery query(q, db);

    if (!query.exec(q)) {
      qWarning() << query.lastError();
      return;
    }

    query.next();
    QString locationName =  query.value("stationName").toString();
    locationNames[locationID] = locationName;
    return;
  } else if (locationID >= 30000001 && locationID <= 31002604) {
    querystr = QString("SELECT "
                       "  solarSystemName AS locName "
                       "FROM "
                       "  mapSolarSystems "
                       "WHERE "
                       "  solarSystemID=%1").arg(locationID);
  } else if (locationID > 1000000000000) {
    QString q { "WITH RECURSIVE tree ( "
                "  ID,parentID,itemID,locationID,keyID,typeID "
                ") "
                "AS ( "
                "  SELECT "
                "    ID,parentID,itemID,locationID,keyID,typeID "
                "  FROM "
                "    Assets "
                "  WHERE "
                "    itemID = %1 "
                "  UNION ALL "
                "  SELECT "
                "    a.ID,a.parentID,a.itemID,"
                "    a.locationID,a.keyID,a.typeID "
                "  FROM Assets a "
                "  INNER JOIN tree t "
                "  ON t.parentID = a.ID AND t.keyID=a.keyID "
                ") "
                "SELECT "
                "  t.ID,t.parentID,t.itemID,t.locationID,t.keyID,t.typeID,l.name "
                "FROM "
                "  tree t "
                "LEFT JOIN Locations l ON "
                "  t.itemID=l.locationID "
                "ORDER BY ID; "
              };

    q = q.arg(locationID);
    QSqlDatabase db = QSqlDatabase::database(":memory:");
    QSqlQuery query(q, db);

    QString location;
    if (!query.exec(q))
      qDebug() << query.lastError();

    while(query.next()) {
      int locID = query.value("locationID").toLongLong();
      if (locID != 0)
        location += locationName(locID);

      location += " > ";
      QString locName = query.value("name").toString();
      QString typeName = typeInfo(query.value("typeID").toInt()).typeName;

      if (!locName.isEmpty() && locName != typeName)
        location += QString("%1 (%2)").arg(locName, typeName);
      else
        location += typeName;
    }

    qDebug() << location;
    locationNames[locationID] = location;
    return;
  } else {
    QString q = QString("SELECT "
                        "  stationName "
                        "FROM "
                        "  ConquerableStations "
                        "WHERE "
                        "  stationID=%1").arg(locationID);
    QSqlDatabase db = QSqlDatabase::database(":memory:");
    QSqlQuery query(q, db);

    if (!query.exec(q)) {
      qWarning() << query.lastError();
      return;
    }

    query.next();
    QString locationName =  query.value("stationName").toString();
    qDebug() << locationName;
    locationNames[locationID] = locationName;
    return;
  }

  query = QSqlQuery(querystr, db);
  query.exec();

  while (query.next())
    locationNames[locationID] = query.value("locName").toString();
}

void StaticData::initProductionType(ProductionType* p)
{
  QSqlDatabase db = getConnection();
  QSqlQuery q(db);
  QString query ("SELECT "
                 "  materialTypeID as materialID,"
                 "  iam.quantity as quantity,"
                 "  iap.typeID as bpID "
                 "FROM "
                 "  industryActivityMaterials iam "
                 "LEFT JOIN industryActivityProducts iap ON "
                 "  (iam.materialTypeID = iap.productTypeID) "
                 "WHERE iam.activityid = 1 AND iam.typeid = %1 "
                 "ORDER BY "
                 "  bpID IS NULL, "
                 "  quantity DESC"
                );

  query = query.arg(p->blueprintID());

  if (!q.exec(query)) {
    qCritical() << q.lastError();
    return;
  }

  p->setPriceInfo(eve::API.priceInfo(p->productID()));

  while (q.next()) {
    long materialID = q.value("materialID").toInt();
    long bpID = q.value("bpID").toInt();
    int quantity = q.value("quantity").toInt();

    ProductionType* pt = new ProductionType {materialID, bpID, quantity, p};
    initProductionType(pt);

    p->addComponent(pt);
  }

  p->refreshPrice();
}

QString StaticData::locationName(qlonglong locationID)
{
  if (locationID <= 0)
    return "";

  if (!locationNames.contains(locationID))
    loadLocationName(locationID);

  return locationNames[locationID];
}

QString StaticData::categoryName(int ID)
{
  return bpCategories_->categoryByID(ID).name;
}

QString StaticData::groupName(int ID)
{
  return bpGroups_->groupByID(ID).name;
}

QString StaticData::marketGroupName(int marketGroupID) const
{
  return marketGroups->data(marketGroupID, MarketGroupsModel::Name).toString();
}

void StaticData::initializeCategoriesModel()
{
  QSqlDatabase db = getConnection();
  if (!db.isOpen())
    return;

  QString querystr("SELECT DISTINCT "
                   "  ic.categoryID AS ID, ic.categoryName AS name "
                   "FROM "
                   "  invCategories ic "
                   "INNER JOIN invGroups ig ON "
                   "  ic.categoryID=ig.categoryID "
                   "INNER JOIN invTypes it ON "
                   "  ig.groupID=it.groupID "
                   "INNER JOIN industryActivityProducts iap ON "
                   "  iap.productTypeID=it.typeID "
                   "WHERE "
                   "  ic.categoryName<>'Blueprint' "
                   "ORDER BY "
                   "  ic.categoryName ASC;");

  QSqlQuery query(db);
  if (!query.exec(querystr))
    qDebug() << query.lastError();

  QVector<invCategory>& categories = bpCategories_->data_;
  categories.clear();

  if (query.size() > 0)
    categories.reserve(query.size());

  while (query.next()) {
    invCategory category;
    category.ID = query.value("id").toInt();
    category.name = query.value("name").toString();

    categories.push_back(category);
  }
}

void StaticData::initializeGroupsModel()
{
  QSqlDatabase db = getConnection();
  if (!db.isOpen())
    return;

  QString querystr("SELECT "
                   "  DISTINCT ig.groupID as ID, ig.groupName as name, "
                   "  ig.categoryID as categoryID "
                   "FROM "
                   "  invGroups ig "
                   "INNER JOIN invTypes it ON "
                   "  ig.groupID=it.groupID "
                   "INNER JOIN industryActivityProducts iap ON "
                   "  iap.productTypeID=it.typeID "
                   "WHERE "
                   "  ig.categoryID<>9 "
                   "ORDER BY "
                   "  ig.groupName ASC");

  QSqlQuery query(querystr, db);
  if (!query.exec(querystr))
    qDebug() << query.lastError();

  QVector<invGroup>& groups = bpGroups_->data_;
  groups.clear();
  if (query.size() < 0)
    groups.reserve(query.size());

  while (query.next()) {
    invGroup group;
    group.ID = query.value("id").toInt();
    group.categoryID = query.value("categoryID").toInt();
    group.name = query.value("name").toString();

    groups.push_back(group);
  }
}

invType StaticData::typeInfo(qlonglong ID)
{
  if (!typeNames.contains(ID))
    loadTypeName(ID);

  return typeNames[ID];
}

RefineInfo StaticData::refineInfo(const invType& type)
{
  QSqlDatabase db = getConnection();

  QString query ("SELECT "
                 "  it.typeID, itm.quantity "
                 "FROM "
                 "  invTypeMaterials itm "
                 "INNER JOIN invTypes it ON "
                 "  itm.materialTypeID=it.typeID "
                 "WHERE "
                 "  itm.typeID=%1 "
                 "ORDER BY "
                 "  it.typeID ASC ");

  QSqlQuery q(query.arg(type.ID), db);

  if (!q.exec())
    qWarning() << q.lastError();

  RefineInfo ri;

  while (q.next()) {
    int id = q.value("typeID").toInt();
    int quantity = q.value("quantity").toInt();
    ri.materials.push_back(QPair<invType, int>(typeInfo(id), quantity));
  }

  return ri;
}

QPixmap StaticData::icon(const invType& type)
{
  QStringList parts = type.iconFile.split("_");
  QString filter =  QString::number(parts.first().toInt()) + "_*_" + QString::number(parts.last().toInt()) + ".*";
  QDir sourceDir("img/", filter);
  QStringList entries = sourceDir.entryList();

  if (entries.isEmpty()) {
    qWarning() << "There is no Icon file for " << type.typeName << type.iconFile;
    return QPixmap();
  }

  QPixmap img (sourceDir.entryInfoList().last().absoluteFilePath());
  return img;
}

BPGroupsModel* StaticData::bpGroupModel()
{
  //WARNING deprecated
  if (bpGroups_->isEmpty() == 0)
    initializeGroupsModel();

  return new BPGroupsModel();
}

BPCategoriesModel* StaticData::bpCategoriesModel()
{
  //WARNING deprecated
  if (bpCategories_->isEmpty())
    initializeCategoriesModel();

  return new BPCategoriesModel();
}

MarketGroupsModel* StaticData::marketGroupsModel()
{
  return marketGroups;
}

IndustryActivityTypeModel* StaticData::industryActivityModel()
{
  return industryActivities;
}

int StaticData::productTypeIDbyBPTypeID(int typeID)
{
  QSqlDatabase db = getConnection();
  if (!db.isOpen())
    return 0;

  QString querystr = QString("SELECT "
                             "  productTypeID "
                             "FROM "
                             "  industryActivityProducts "
                             "WHERE "
                             "  typeID=%1").arg(typeID);

  QSqlQuery query(db);
  if (!query.exec(querystr))
    qDebug() << query.lastError();

  int productID = 0;

  if(query.next())
    productID = query.value("productTypeID").toInt();

  return productID;
}

void StaticData::loadTypeName(qlonglong typeID)
{
  QSqlDatabase db = getConnection();
  if (!db.isOpen()) {
    invType type;
    typeNames[typeID] = type;
    return;
  }

  QString querystr = QString("SELECT "
                             "  it.typeID, "
                             "  it.groupID, "
                             "  ig.categoryID, "
                             "  it.typeName, "
                             "  it.volume,  "
                             "  it.marketGroupID, "
                             "  coalesce(dta.valueInt, dta.valueFloat) AS metaLevel, "
                             "  im.metaGroupID, "
                             "  eI.iconFile "
                             "FROM invTypes it "
                             "INNER JOIN invGroups ig ON "
                             "  it.groupID=ig.groupID "
                             "LEFT  JOIN invMetaTypes im ON "
                             "  it.typeID=im.typeID "
                             "LEFT  JOIN eveIcons eI "
                             "  ON it.iconID=eI.iconID "
                             "LEFT  JOIN dgmTypeAttributes dta ON "
                             "  dta.typeID=it.typeID AND "
                             "  dta.attributeID=633 "
                             "WHERE "
                             "  it.typeID=%1").arg(typeID);
  QSqlQuery query(db);
  if (!query.exec(querystr))
    qDebug() << query.lastError();


  while (query.next()) {
    invType type;
    type.ID          = query.value("typeID").toInt();
    type.groupID     = query.value("groupID").toInt();
    type.categoryID  = query.value("categoryID").toInt();
    type.typeName    = query.value("typeName").toString();
    type.marketGroup = query.value("marketGroupID").toInt();
    type.volume      = query.value("volume").toDouble();
    type.iconFile    = query.value("iconFile").toString();
    type.metaLevel   = query.value("metaLevel").toInt();

    if (query.value("metaGroupID").isNull())
      type.metaGroupID = 1;
    else
      type.metaGroupID = query.value("metaGroupID").toInt();

    typeNames[typeID] = type;
  }

}

QSqlDatabase StaticData::getConnection()
{
  QSqlDatabase db = QSqlDatabase::database("SDE");

  if (!db.isOpen()) {
    db = QSqlDatabase::addDatabase("QSQLITE", "SDE");
    QString name = dbLocation;
    db.setDatabaseName(name);
    if (!db.open())
      qDebug() << db.lastError();
  }

  return db;
}

}
