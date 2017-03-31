#include "locationsxmlparser.h"

#include <QtXml>
#include <QtSql>
#include <QDebug>

namespace eve {

LocationsXmlParser::LocationsXmlParser(bool clearCache) :
  XmlParser(), clearCache_(clearCache)
{

}

void LocationsXmlParser::parseInternal(const QByteArray& xml, ApiKeyInfo* key)
{
  if (xml.isEmpty()) {
    qDebug() << "Empty XML Data";
    return;
  }

  QVariantList itemIDs;
  QVariantList itemNames;
  QDateTime cachedUntil;

  QXmlStreamReader reader(xml);

  while (!reader.atEnd() && !reader.hasError()) {
    reader.readNext();

    if (reader.isStartDocument())
      continue;

    if (reader.isStartElement()) {
      if (reader.name() == "cachedUntil") {
        QString time = reader.readElementText();
        cachedUntil = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");
      }

      if (reader.name() == "row") {
        QXmlStreamAttributes attr = reader.attributes();

        itemIDs << attr.value("itemID").toString();
        itemNames << attr.value("itemName").toString();
      }
    }
  }

  if (reader.hasError()) {
    qWarning() << reader.errorString();
    QFile file("ErrorXML-" + QTime::currentTime().toString("dd-MM-hh-mm-ss") + ".xml");
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
      qWarning() << "Fie not opened";
      return;
    }
    QDataStream stream(&file);
    stream << xml;
    return;
  }

  QSqlDatabase db;
  db = QSqlDatabase::database(":memory:");


  QSqlQuery q(db);
  if (!db.transaction()) {
    qDebug() << db.lastError();
    return;
  }

  if (clearCache_) {
    qDebug() << "Clearing locations cache";
    if (!q.exec(QString("DELETE FROM Locations "
                        "WHERE keyID=%1").arg(key->ID())))
      qWarning() << q.lastError();

    clearCache_ = false;
  }

  q.prepare(QString("INSERT OR IGNORE INTO Locations "
                    "VALUES (?, ?, %1)").arg(key->ID()));
  q.addBindValue(itemIDs);
  q.addBindValue(itemNames);

  if (!q.execBatch())
    qWarning() << q.lastError();

  const QString updateKeyQuery = "UPDATE OR IGNORE CacheTimes "
                                 "SET locations=:Time "
                                 "WHERE keyID=:keyID";

  const QString insertKeyQuery = "INSERT OR IGNORE INTO CacheTimes (keyID, locations) "
                                 "VALUES(:keyID, :Time) ";

  q.prepare(updateKeyQuery);
  q.bindValue(":Time", cachedUntil.toString(Qt::ISODate));
  q.bindValue(":keyID", key->ID());

  if (!q.exec())
    qWarning() << "Update bp cache time error -" << q.lastError();

  q.prepare(insertKeyQuery);
  q.bindValue(":Time", cachedUntil.toString(Qt::ISODate));
  q.bindValue(":keyID", key->ID());

  if (!q.exec())
    qWarning() << q.lastError();

  if (!db.commit()) {
    qWarning() << db.lastError();
    clearCache_ = true;
    db.rollback();
  }
}

} // namespace eve
