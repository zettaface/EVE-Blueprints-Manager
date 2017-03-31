#include "conquerablestationxmlparser.h"

#include <QtXml>
#include <QtSql>
#include <QDebug>

namespace eve {

void ConquerableStationXmlParser::parseInternal(const QByteArray& xml, ApiKeyInfo* key)
{
  if (xml.isEmpty()) {
    qDebug() << "Empty XML Data";
    return;
  }

  QSqlDatabase db;
  db = QSqlDatabase::database(":memory:");

  QVariantList stationIDs;
  QVariantList stationNames;
  QVariantList stationTypeIDs;
  QVariantList solarSystemIDs;
  QVariantList corporationIDs;
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

        stationIDs << attr.value("stationID").toString();
        stationNames << attr.value("stationName").toString();
        stationTypeIDs << attr.value("stationTypeID").toString();
        solarSystemIDs << attr.value("solarSystemID").toString();
        corporationIDs << attr.value("corporationID").toString();
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

  if (!q.exec(QString("DELETE FROM ConquerableStations")))
    qDebug() << q.lastError();

  q.prepare(QString("INSERT OR IGNORE INTO ConquerableStations VALUES (?, ?, ?, ?, ?)"));
  q.addBindValue(stationIDs);
  q.addBindValue(stationNames);
  q.addBindValue(stationTypeIDs);
  q.addBindValue(solarSystemIDs);
  q.addBindValue(corporationIDs);

  if (!q.execBatch())
    qDebug() << q.lastError();

  if (!db.commit()) {
    qDebug() << db.lastError();
    db.rollback();
  }
}

} // namespace eve
