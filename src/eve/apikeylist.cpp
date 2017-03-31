#include "apikeylist.h"
#include <QtSql>

#include <QDebug>

namespace eve {

ApiKeyList::ApiKeyList(QObject* parent) :
  QAbstractListModel(parent)
{

}

int ApiKeyList::rowCount(const QModelIndex& parent) const
{
  return keys.size();
}

int ApiKeyList::columnCount(const QModelIndex& parent) const
{
  return static_cast<int>(ColumnCount);
}

QVariant ApiKeyList::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  ApiKeyInfo* key = keys[index.row()];
  if (role == Qt::DisplayRole) {
    switch(index.column()) {
      case Name:
        if (key->type() == "char")
          return key->charName();
        else
          return key->corpName();
      case Type:
        return key->type();
      case KeyID:
        return key->keyID();
      case vCode:
        return key->vCode();
      case CharID:
        return key->charID();
      default:
        break;
    }
  }

  return QVariant();
}

QVariant ApiKeyList::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole) {
    switch(section) {
      case Name:
        return "Name";
      case Type:
        return "Key Type";
      case KeyID:
        return "Key ID";
      case vCode:
        return "Verification Code";
      default:
        break;
    }
  }

  return QVariant();

}

void ApiKeyList::addKey(ApiKeyInfo* key)
{
  beginInsertRows(QModelIndex(), keys.size(), keys.size());
  keys.push_back(key);
  endInsertRows();
}

void ApiKeyList::removeKey(const ApiKeyInfo* key)
{
  const_iterator it = qFind(keys, key);

  if (it == keys.end())
    return;

  int pos = it - keys.begin();
  beginRemoveRows(QModelIndex(), pos, pos);
  keys.removeAt(pos);
  endRemoveRows();
}

ApiKeyInfo* ApiKeyList::key(int i) const
{
  return keys.at(i);
}

ApiKeyInfo* ApiKeyList::keyByKeyID(int id) const
{
  auto it = std::find_if(keys.begin(), keys.end(), [id](ApiKeyInfo * key) {
    if (key->ID() == id)
      return true;

    return false;
  });

  if (it == keys.end())
    return nullptr;

  return *it;
}

bool ApiKeyList::contains(ApiKeyInfo* key) const
{
  return keys.contains(key);
}

ApiKeyList::Container ApiKeyList::getKeys() const
{
  return keys;
}

void ApiKeyList::setKeys(const Container& value)
{
  if (value.isEmpty())
    return;

  beginResetModel();
  keys.clear();
  endResetModel();

  beginInsertRows(QModelIndex(), 0, value.size() - 1);
  keys = value;
  endInsertRows();
}

bool ApiKeyList::isEmpty() const
{
  return keys.isEmpty();
}

bool ApiKeyList::removeRows(int row, int count, const QModelIndex& parent)
{
  QSqlDatabase db = QSqlDatabase::database(":memory:");
  QSqlQuery q(db);

  QStringList queries {
    "DELETE FROM ApiKeys WHERE id=%1",
    "DELETE FROM Assets WHERE keyID=%1",
    "DELETE FROM Blueprints WHERE keyID=%1",
    "DELETE FROM BlueprintsFilter WHERE keyID=%1",
    "DELETE FROM Locations WHERE keyID=%1",
    "DELETE FROM CacheTimes WHERE keyID=%1"
  };

  beginRemoveRows(parent, row, row + count - 1);
  db.transaction();

  for (int i = 0; i < count; i++) {
    for (const QString& query : queries) {
      if (!q.exec(query.arg(keys[row + i]->ID()))) {
        qWarning() << "DB error:" << q.lastError();
        db.rollback();
        endRemoveRows();

        return false;
      }
    }
  }

  if (!db.commit())
    qWarning() << "Transaction error:" << db.lastError();

  keys.remove(row, count);
  endRemoveRows();
  emit keysChanged();

  return true;
}

void ApiKeyList::fetchFromDB()
{
  //TODO Merge two queries into one
  QSqlDatabase db = QSqlDatabase::database(":memory:");
  QSqlQuery q(db);

  if (!q.exec("SELECT * FROM ApiKeys")) {
    qDebug() << q.lastError();
    return;
  }

  keys.clear();

  nextBlueprintsFetchTime_   = QDateTime({0, 0, 0});
  nextAssetsFetchTime_       = QDateTime({0, 0, 0});
  nextIndustryJobsFetchTime_ = QDateTime({0, 0, 0});

  while (q.next()) {
    ApiKeyInfo* key = new ApiKeyInfo(q.record());
    addKey(key);
  }

  for(ApiKeyInfo* key : keys) {
    q.exec(QString("SELECT * FROM CacheTimes WHERE keyID=%1").arg(key->ID()));

    while (q.next()) {
      QDateTime blueprintsTime = q.value("blueprints").toDateTime();
      QDateTime assetsTime = q.value("assets").toDateTime();
      QDateTime industryJobsTime = q.value("industryJobs").toDateTime();

      updateTime(nextBlueprintsFetchTime_, blueprintsTime);
      updateTime(nextAssetsFetchTime_, assetsTime);
      updateTime(nextIndustryJobsFetchTime_, industryJobsTime);

      key->blueprintsCachedUntil_ = blueprintsTime;
      key->assetCachedUntil_ = assetsTime;
      key->industryJobsCachedUntil_ = industryJobsTime;
    }
  }
}

void ApiKeyList::updateTime(QDateTime& oldTime, const QDateTime& newTime)
{
  if (newTime < oldTime || !oldTime.isValid())
    oldTime = newTime;
}

}
