#include "api.h"
#include <QEventLoop>
#include <QtSql>
#include <QtXml>

#include <algorithm>
#include <tuple>

#include "apirequest.h"
#include "crest.h"
#include "imageserverrequest.h"
#include "staticdata.h"
#include "keysrequest.h"

namespace eve {

Api API;

Api::Api(QObject* parent) : QObject(parent)
{
  keys_ = new ApiKeyList;
  connect(&downloadManager, SIGNAL(finished()), this, SIGNAL(fetchFinished()));

  keyStatusParser = QSharedPointer<ApiKeyUpdater>::create();
  connect(keyStatusParser.data(), SIGNAL(parsed(ApiKeyInfo*)), this, SLOT(proceedKeysChecking()));
}

Api::~Api()
{

}

void Api::init()
{
  prepareMemoryDB();
  keys_->fetchFromDB();
  connect(&downloadManager, SIGNAL(finished()), keys_, SLOT(fetchFromDB()));
}

int Api::queueAssetsFetch()
{
  auto assetParser = QSharedPointer<AssetXmlParser>::create();
  connect(assetParser.data(), SIGNAL(parsed(ApiKeyInfo*)), this, SIGNAL(assetReqParsed()));
  connect(assetParser.data(), SIGNAL(parsed(ApiKeyInfo*)), this, SLOT(queueLocationsFetch(ApiKeyInfo*)));

  return queueAssetsFetch(assetParser);
}

int Api::queueAssetsFetch(QSharedPointer<XmlParser> parser)
{
  const QString uri = "/Assetlist.xml.aspx";
  int count = 0;

  for (ApiKeyInfo* key : *keys_) {
    if (key->isAssetsExpired()) {
      downloadManager.append(new ApiRequest(uri, key, parser));
      ++count;
    }
  }

  return count;
}

int Api::queueBlueprintsFetch()
{
  auto bpParser = QSharedPointer<BlueprintsXmlParser>::create();
  connect(bpParser.data(), SIGNAL(parsed(ApiKeyInfo*)), this, SIGNAL(blueprintReqParsed()));

  return queueBlueprintsFetch(bpParser);
}

int Api::queueBlueprintsFetch(QSharedPointer<XmlParser> parser)
{
  const QString uri = "/Blueprints.xml.aspx";
  int count = 0;

  for (ApiKeyInfo* key : *keys_) {
    if (key->isBlueprintsExpired()) {

      downloadManager.append(new ApiRequest(uri, key, parser));
      ++count;
    }
  }

  return count;

}

int Api::queueIndustryJobsFetch()
{
  auto jobsParser = QSharedPointer<IndustryJobsXmlParser>::create();
  connect(jobsParser.data(), SIGNAL(parsed()), this, SIGNAL(industryJobReqParsed()));

  return queueIndustryJobsFetch(jobsParser);
}

int Api::queueIndustryJobsFetch(QSharedPointer<XmlParser> parser)
{
  const QString uri = "/IndustryJobs.xml.aspx";
  int count = 0;

  for (ApiKeyInfo* key : *keys_) {
    if (key->isIndustryJobsExpired()) {
      downloadManager.append(new ApiRequest(uri, key, parser));
      ++count;
    }
  }

  return count;
}

void Api::queueConqStationsFetch()
{
  const QString uri = "/ConquerableStationList.xml.aspx";
  downloadManager.append(new ApiRequest(uri, "eve", nullptr, QSharedPointer<ConquerableStationXmlParser>::create()));
}


void Api::queueAllTypeIconsFetch()
{
  QSqlDatabase db = SDE.getConnection();
  QSqlQuery q(db);
  q.exec("SELECT DISTINCT iam.materialTypeID FROM industryActivityMaterials iam "
         "UNION "
         "SELECT DISTINCT ip.productTypeID "
         "FROM industryActivityProducts ip "
         "INNER JOIN invTypes it ON ip.productTypeID=it.typeID "
         "WHERE it.published=1 "
         "UNION "
         "SELECT DISTINCT ip.typeID "
         "FROM industryActivityProducts ip "
         "INNER JOIN invTypes it ON ip.productTypeID=it.typeID "
         "WHERE it.published=1");

  queueTypeIconFetch(0);
  while (q.next()) {
    int typeID = q.value(0).toInt();
    queueTypeIconFetch(typeID);
  }

}

void Api::queueTypeIconFetch(long typeID)
{
  Request* req = new ImageServerRequest("Type", typeID, 64, QSharedPointer<InventoryTypeImageParser>::create());
  req->setPriority(-5);
  downloadManager.append(req);
}

int Api::queueLocationsFetch(ApiKeyInfo* key)
{
  auto parser = QSharedPointer<LocationsXmlParser>::create(true);
  connect(parser.data(), SIGNAL(parsed(ApiKeyInfo*)), this, SIGNAL(locationReqParsed()));

  return queueLocationsFetch(key);
}

int Api::queueLocationsFetch(ApiKeyInfo* key, QSharedPointer<XmlParser> parser)
{
  QSqlDatabase db = QSqlDatabase::database(":memory:");
  QSqlQuery q(db);
  QString query = QString("SELECT "
                          "  DISTINCT a.itemID "
                          "FROM "
                          "  Assets a "
                          "LEFT JOIN "
                          "  Blueprints bp "
                          "ON "
                          "  a.itemID=bp.itemID "
                          "WHERE "
                          "  a.singleton=1 AND "
                          "  a.keyID=%1 AND "
                          "  bp.itemID IS NULL ").arg(key->ID());

  if (!q.exec(query)) {
    qWarning() << q.lastError();
    return false;
  }

  QStringList locationIDs;
  const int idPerRequest = 70;
  int count = 0;

  QString uri = "/Locations.xml.aspx";

  while(q.next()) {
    if (locationIDs.size() >= idPerRequest) {

      downloadManager.append(new ApiRequest(uri, key, parser, {{"IDs", locationIDs.join(',')}}));
      locationIDs.clear();
      count++;
    }

    locationIDs << QString::number(q.value("itemID").toLongLong());
  }

  if (locationIDs.empty()) {
    emit locationRequestsAdded(count);
    return count;
  }

  downloadManager.append(new ApiRequest(uri, key, parser, {{"IDs", locationIDs.join(',')}}));
  count++;

  queueConqStationsFetch();
  emit locationRequestsAdded(count);

  return count;
}


void Api::updatePrices(QVector<PriceInfo*> prices)
{
  for (PriceInfo* p : prices) {
    if (prices_.contains(p->typeID())) {
      prices_[p->typeID()]->initFrom(*p);
      prices_[p->typeID()]->updatePrices();
      delete p;
    } else
      prices_.insert(p->typeID(), p);
  }

}

void Api::proceedKeysChecking()
{
  keysChecked_++;

  if (keysChecked_ == keysToCheck_) {
    if (keyStatusParser->expiredKeys().size() > 0)
      showExpiredKeysDialog();

    bool hasExpiredKeys = keyStatusParser->expiredKeys().size() > 0;
    emit keysChecked(hasExpiredKeys);
  }
}

void Api::showExpiredKeysDialog() const
{

}

void Api::prepareMemoryDB()
{
  qRegisterMetaType<ApiKeyInfo*>();
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", ":memory:");
  db.setDatabaseName("data.sqlite");
  db.open();

  QSqlQuery q(db);
  q.exec("PRAGMA synchronous=OFF");
  q.exec("PRAGMA count_changes=OFF");
  q.exec("PRAGMA temp_store=MEMORY");

  q.exec("CREATE TABLE IF NOT EXISTS ApiKeys ("
         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
         "keyID INTEGER, "
         "vcode TEXT, "
         "charID INTEGER, "
         "charName TEXT, "
         "corpID INTEGER, "
         "corpName TEXT, "
         "allianceID INTEGER, "
         "allianceName TEXT, "
         "accessMask INTEGER, "
         "type TEXT, "
         "UNIQUE (keyID, charID))"
        );

  q.exec("CREATE TABLE IF NOT EXISTS Blueprints ("
         "  itemID INTEGER, "
         "  locationID INTEGER, "
         "  typeName TEXT, "
         "  typeID INTEGER, "
         "  ME INTEGER, "
         "  TE INTEGER, "
         "  runs INTEGER, "
         "  keyID INTEGER, "
         "  PRIMARY KEY ("
         "    itemID, "
         "    keyID"
         "  )"
         ")");

  q.exec("CREATE TABLE IF NOT EXISTS Assets ("
         "  ID INTEGER, "
         "  itemID INTEGER, "
         "  typeID INTEGER, "
         "  locationID INTEGER, "
         "  parentID INTEGER, "
         "  quantity INTEGER, "
         "  flag INTEGER, "
         "  singleton BOOLEAN, "
         "  keyID INTEGER, "
         "  PRIMARY KEY ("
         "    ID, "
         "    keyID"
         "  )"
         ")"
        );

  q.exec("CREATE TABLE IF NOT EXISTS CacheTimes ("
         "  keyID INTEGER ASC NOT NULL,"
         "  assets     TEXT,"
         "  blueprints TEXT,"
         "  locations  TEXT,"
         "  industryJobs  TEXT,"
         "  PRIMARY KEY ("
         "    keyID"
         "  )"
         ");");

//  q.exec("CREATE TABLE IF NOT EXISTS BlueprintsFilter ("
//         "  locationID INTEGER, "
//         "  keyID INTEGER, "
//         "  PRIMARY KEY ("
//         "    locationID, "
//         "    keyID"
//         "  )"
//         ")");

  q.exec("CREATE TABLE IF NOT EXISTS BlueprintsColors ("
         "fgcolor TEXT, "
         "bgcolor TEXT, "
         "filter TEXT, "
         "priority INTEGER"
         ")");

  q.exec("CREATE TABLE IF NOT EXISTS IndustryJobs ("
         "jobID INTEGER, "
         "activityID INTEGER, "
         "status INTEGER, "
         "runs INTEGER, "
         "licensedRuns INTEGER, "
         "outputLocationID INTEGER, "
         "installerID INTEGER, "
         "installerName INTEGER, "
         "facilityID INTEGER, "
         "blueprintID INTEGER, "
         "blueprintTypeID INTEGER, "
         "probability TEXT, "
         "startDate TEXT, "
         "endDate TEXT, "
         "completedDate TEXT, "
         "keyID INTEGER, "
         "PRIMARY KEY ("
         "  jobID,"
         "  keyID"
         "))");

  q.exec("CREATE TABLE IF NOT EXISTS Locations ("
         "locationID INTEGER, "
         "name TEXT, "
         "keyID INTEGER, "
         "PRIMARY KEY (locationID, keyID))");

  q.exec("CREATE TABLE IF NOT EXISTS ConquerableStations ("
         "stationID INTEGER, "
         "stationName TEXT, "
         "stationTypeID INTEGER, "
         "solarSystemID INTEGER, "
         "corporationID INTEGER, "
         "PRIMARY KEY (stationID))");

  q.exec("CREATE TABLE IF NOT EXISTS InventoryTypeImages ("
         "typeID INTEGER, "
         "image BLOB, "
         "PRIMARY KEY (typeID))");

//  queueAllTypeIconsFetch();
}

void Api::clearCache()
{
  QSqlDatabase db = QSqlDatabase::database(":memory:");
  db.transaction();

  QSqlQuery q(db);

  if (!q.exec("DELETE FROM Blueprints"))
    qDebug() << q.lastError();
  if (!q.exec("DELETE FROM Assets"))
    qDebug() << q.lastError();
  if (!q.exec("DELETE FROM CacheTimes"))
    qDebug() << q.lastError();

  if (!q.exec("DELETE FROM Locations"))
    qDebug() << q.lastError();

  if (!db.commit()) {
    qDebug() << q.lastError();
    db.rollback();
  }

  keys_->fetchFromDB();
}

void Api::updateApiKeys()
{
  keyStatusParser->clearExpiredKeys();

  updateApiKeys(keyStatusParser);
}

void Api::updateApiKeys(QSharedPointer<Parser> parser)
{
  keysToCheck_ = keys_->size();
  keysChecked_ = 0;

  for (ApiKeyInfo* key : *keys_)
    downloadManager.append(new ApiRequest("/apikeyinfo.xml.aspx", "account", key, parser));

}


void Api::fetchKeys(const QString& key, const QString& vcode)
{
  auto parser = QSharedPointer<KeyInfoXmlParser>::create();
  connect(parser.data(), SIGNAL(finished(QList<ApiKeyInfo>)), this, SIGNAL(keysFetched(QList<ApiKeyInfo>)));

  fetchKeys(key, vcode, parser);
}

void Api::fetchKeys(const QString& key, const QString& vcode, QSharedPointer<Parser> parser)
{
  downloadManager.append(new KeysRequest(key, vcode, parser));
}

void Api::saveKey(ApiKeyInfo* key)
{
  QSqlDatabase db = QSqlDatabase::database(":memory:");
  QSqlQuery q(db);
  q.prepare("INSERT OR IGNORE INTO ApiKeys ("
            "  keyID, "
            "  vCode, "
            "  charID, "
            "  charName, "
            "  corpID, "
            "  corpName, "
            "  allianceID, "
            "  allianceName, "
            "  type"
            ") "
            "VALUES (:keyID, :vCode, :charID, :charName, "
            ":corpID, :corpName, :allianceID, :allianceName, "
            ":type)");

  q.bindValue(":keyID", key->keyID());
  q.bindValue(":vCode", key->vCode());
  q.bindValue(":charID", key->charID());
  q.bindValue(":charName", key->charName());
  q.bindValue(":corpID", key->corpID());
  q.bindValue(":corpName", key->corpName());
  q.bindValue(":allianceID", key->allianceID());
  q.bindValue(":allianceName", key->allianceName());
  q.bindValue(":type", key->type());

  if (!q.exec())
    qDebug() << "Saving APIKEY error: " << q.lastError();

  if (!keys_->contains(key))
    keys_->addKey(key);

  keys_->fetchFromDB();
}

void Api::removeKey(ApiKeyInfo* key)
{
  removeKeyById(key->ID());
}

void Api::removeKeyById(int keyID)
{
  QStringList queries {
    "DELETE FROM ApiKeys WHERE id=%1",
    "DELETE FROM Assets WHERE keyID=%1",
    "DELETE FROM Blueprints WHERE keyID=%1",
    "DELETE FROM BlueprintsFilter WHERE keyID=%1",
    "DELETE FROM Locations WHERE keyID=%1",
    "DELETE FROM IndustryJobs WHERE keyID=%1",
    "DELETE FROM CacheTimes WHERE keyID=%1"
  };

  QSqlDatabase db = QSqlDatabase::database(":memory:");
  QSqlQuery q(db);

  db.transaction();

  for (const QString& query : queries) {
    if (!q.exec(query.arg(keyID)))
      qWarning() << q.lastError();
  }

  if (!db.commit()) {
    db.rollback();
    qWarning() << db.lastError();
  }

  keys_->fetchFromDB();
}

ApiKeyList* Api::keys() const
{
  return keys_;
}

PriceInfo* Api::priceInfo(long typeID)
{
  if (prices_.contains(typeID))
    return prices_[typeID];

  PriceInfo* price = new PriceInfo(typeID);
  prices_.insert(typeID, price);

  downloadManager.append(CrestApi::market(10000002).orders(typeID).toRequest(QSharedPointer<MarketOrdersParser>::create(price)));

  return price;
}

QString Api::currentFetchRequest() const
{
  return "";
}

QPixmap Api::typeIcon(long typeID) const
{
  QSqlDatabase db = QSqlDatabase::database(":memory:");
  QSqlQuery q(db);

  q.exec(QString("SELECT image FROM InventoryTypeImages WHERE typeID=%1").arg(typeID));
  if (!q.next()) {
    eve::API.queueTypeIconFetch(typeID);
    return QPixmap();
  }

  QByteArray raw = q.value(0).toByteArray();

  QPixmap icon = QPixmap::fromImage(QImage::fromData(raw));
  return icon;
}

const QVector<ApiKeyUpdater::Error>& Api::keyCheckErrors() const
{
  return keyStatusParser->expiredKeys();
}

} //namespace eve
