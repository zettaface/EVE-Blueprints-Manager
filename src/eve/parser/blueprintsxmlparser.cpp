#include "blueprintsxmlparser.h"

#include <QtXml>
#include <QtSql>
#include <QDebug>

namespace eve {

void BlueprintsXmlParser::parseInternal(const QByteArray& xml, ApiKeyInfo* key)
{
  if (xml.isEmpty()) {
    qDebug() << "Empty XML Data";
    return;
  }

  QSqlDatabase db;
  db = QSqlDatabase::database(":memory:");

  QVariantList itemIDs;
  QVariantList locationIDs;
  QVariantList typeNames;
  QVariantList typeIDs;
  QVariantList MEs;
  QVariantList TEs;
  QVariantList runs;
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
        cachedUntil.setTimeSpec(Qt::UTC);
      }

      if (reader.name() == "row") {
        QXmlStreamAttributes attr = reader.attributes();

        itemIDs << attr.value("itemID").toString();
        locationIDs << attr.value("locationID").toString();
        typeNames << attr.value("typeName").toString();
        typeIDs << attr.value("typeID").toString();
        MEs << attr.value("materialEfficiency").toString();
        TEs << attr.value("timeEfficiency").toString();
        runs << attr.value("runs").toString();
      }
    }
  }

  if (reader.hasError()) {
    qDebug() << reader.errorString();
    return;
  }

  QSqlQuery q(db);
  if (!db.transaction()) {
    qDebug() << db.lastError();
    return;
  }

  if (!q.exec(QString("DELETE FROM Blueprints WHERE keyID=%1").arg(key->ID())))
    qDebug() << q.lastError();

  q.prepare(QString("INSERT OR IGNORE INTO Blueprints VALUES (?, ?, ?, ?, ?, ?, ?, %1)").arg(key->ID()));
  q.addBindValue(itemIDs);
  q.addBindValue(locationIDs);
  q.addBindValue(typeNames);
  q.addBindValue(typeIDs);
  q.addBindValue(MEs);
  q.addBindValue(TEs);
  q.addBindValue(runs);

  if (!q.execBatch())
    qDebug() << q.lastError();

  const QString updateKeyQuery = "UPDATE OR IGNORE CacheTimes "
                                 "SET blueprints=:Time "
                                 "WHERE keyID=:keyID";

  const QString insertKeyQuery = "INSERT OR IGNORE INTO CacheTimes (keyID, blueprints) "
                                 "VALUES(:keyID, :Time) ";

  q.prepare(updateKeyQuery);
  q.bindValue(":Time", cachedUntil.toString(Qt::ISODate));
  q.bindValue(":keyID", key->ID());

  if (!q.exec())
    qDebug() << "Update bp cache time error -" << q.lastError();

  q.prepare(insertKeyQuery);
  q.bindValue(":Time", cachedUntil.toString(Qt::ISODate));
  q.bindValue(":keyID", key->ID());


  if (!q.exec())
    qDebug() << q.lastError();

  if (!db.commit()) {
    qDebug() << db.lastError();
    db.rollback();
  }

  qDebug() << "Blueprints for " << key->ID() << " parsed";
}

} // namespace eve
