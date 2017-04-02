#include "assetxmlparser.h"

#include <QtXml>
#include <QtSql>
#include <QDebug>

namespace eve {

void AssetXmlParser::parseInternal(const QByteArray& xml, ApiKeyInfo* key)
{
  if (xml.isEmpty()) {
    qDebug() << "Empty xml";
    return;
  }

  struct AssetNode
  {
    int ID {-1};
    int lastChildID {-1};
    int parentID {-1};
  };

  QXmlStreamReader reader(xml);

  QDateTime cachedUntil;
  QVariantList IDs;
  QVariantList itemIDs;
  QVariantList typeIDs;
  QVariantList locations;
  QVariantList parents;
  QVariantList quantities;
  QVariantList flags;
  QVariantList singletons;

  QHash<int, AssetNode> nodeMap;

  AssetNode root;
  int count = 1;
  root.ID = 0;

  nodeMap.insert(0, root);

  while (!reader.atEnd() && !reader.hasError()) {
    reader.readNext();

    if (reader.isStartDocument())
      continue;

    if (reader.isStartElement()) {
      if (reader.name() == "cachedUntil") {
        QString time = reader.readElementText();
        cachedUntil = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");
        cachedUntil.setTimeSpec(Qt::UTC);
        continue;
      }

      if (reader.name() == "error") {
        reader.attributes().value("code").toInt();
        //TODO: api error handling
        return;
      }

      if (reader.name() == "rowset") {
        if (reader.attributes().value("name").toString() == "assets")
          continue;

        if (reader.attributes().value("name").toString() == "contents") {
          root = nodeMap[root.lastChildID];
          continue;
        }
      }

      if (reader.name() == "row") {
        QXmlStreamAttributes attr = reader.attributes();

        AssetNode node;// = new AssetNode;
        node.ID = count;
        node.parentID = root.ID;
        root.lastChildID = node.ID;

        nodeMap.insert(node.ID, node);

        IDs << node.ID;// node->ID;
        parents << node.parentID;
        itemIDs << attr.value("itemID").toLongLong();
        typeIDs << attr.value("typeID").toInt();
        quantities << attr.value("quantity").toInt();
        flags << attr.value("flag").toInt();
        singletons << attr.value("singleton").toInt();

        if (attr.hasAttribute("locationID"))
          locations << attr.value("locationID").toLongLong();
        else
          locations << QVariant();

        count++;
      }
    }

    if (reader.isEndElement()) {
      if (reader.name() == "rowset") {
        if (root.parentID >= 0)
          root = nodeMap[root.parentID];
      }

    }

  }

  if (reader.hasError()) {
    qWarning() << "XML Parsing error -" << reader.errorString();
    return;
  }

  QSqlDatabase db = QSqlDatabase::database(":memory:");
  QSqlQuery q(db);
  db.transaction();

  if (!q.exec(QString("DELETE FROM Assets WHERE keyID=%1").arg(key->ID()))) {
    qWarning() << q.lastError();
    return;
  }

  q.prepare(QString("INSERT OR REPLACE INTO Assets "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, %1)").arg(key->ID()));
  q.addBindValue(IDs);
  q.addBindValue(itemIDs);
  q.addBindValue(typeIDs);
  q.addBindValue(locations);
  q.addBindValue(parents);
  q.addBindValue(quantities);
  q.addBindValue(flags);
  q.addBindValue(singletons);

  if (!q.execBatch())
    qWarning() << "VALUES INSERTION ERROR -" << q.lastError();

  const QString updateKeyQuery = "UPDATE OR IGNORE CacheTimes "
                                 "SET assets=:Time "
                                 "WHERE keyID=:keyID";

  const QString insertKeyQuery = "INSERT OR IGNORE INTO CacheTimes (keyID, assets) "
                                 "VALUES(:keyID, :Time) ";

  q.prepare(updateKeyQuery);
  q.bindValue(":Time", cachedUntil.toString(Qt::ISODate));
  q.bindValue(":keyID", key->ID());

  if (!q.exec())
    qWarning() << "Update asset cache timer error -" << q.lastError();

  q.prepare(insertKeyQuery);
  q.bindValue(":Time", cachedUntil.toString(Qt::ISODate));
  q.bindValue(":keyID", key->ID());

  if (!q.exec())
    qWarning() << "Update asset cache timer error -" << q.lastError();

  if (!db.commit()) {
    qWarning() << "Transaction commit error -" << db.lastError();
    db.rollback();
  }

  emit finished(key);
}

} // namespace eve
