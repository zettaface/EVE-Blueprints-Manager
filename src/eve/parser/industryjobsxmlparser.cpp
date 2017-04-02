#include "industryjobsxmlparser.h"

#include <QtXml>
#include <QtSql>
#include <QDebug>

namespace eve {

void IndustryJobsXmlParser::parseInternal(const QByteArray& xml, ApiKeyInfo* key)
{
  if (xml.isEmpty()) {
    qDebug() << "Empty XML Data";
    return;
  }

  QSqlDatabase db;
  db = QSqlDatabase::database(":memory:");

  QVariantList jobIDs;
  QVariantList activityIDs;
  QVariantList statuses;
  QVariantList runs;
  QVariantList licensedRuns;
  QVariantList outputLocations;
  QVariantList installerIDs;
  QVariantList installerNames;
  QVariantList facilityIDs;
  QVariantList blueprintIDs;
  QVariantList blueprintTypeIDs;
  QVariantList probabilities;
  QVariantList startDates;
  QVariantList endDates;
  QVariantList completedDates;
//  QVariantList outputLocations;
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
        continue;
      }

      if (reader.name() == "row") {
        QXmlStreamAttributes attr = reader.attributes();

        jobIDs << attr.value("jobID").toString();
        activityIDs << attr.value("activityID").toString();
        statuses << attr.value("status").toString();
        runs << attr.value("runs").toString();
        licensedRuns << attr.value("licensedRuns").toString();
        outputLocations << attr.value("outputLocation").toString();
        installerIDs << attr.value("installerID").toString();
        installerNames << attr.value("installerName").toString();
        facilityIDs << attr.value("facilityID").toString();
        blueprintIDs << attr.value("blueprintID").toString();
        blueprintTypeIDs << attr.value("blueprintTypeID").toString();
        probabilities << attr.value("probability").toString();
        startDates << attr.value("startDate").toString();
        endDates << attr.value("endDate").toString();
        completedDates << attr.value("completedDate").toString();
      }
    }
  }

  if (reader.hasError()) {
    qWarning() << reader.errorString();
    return;
  }

  QSqlQuery q(db);
  if (!db.transaction()) {
    qWarning() << db.lastError();
    return;
  }

  if (!q.exec(QString("DELETE FROM IndustryJobs WHERE keyID=%1").arg(key->ID())))
    qWarning() << q.lastError();

  q.prepare(QString("INSERT OR IGNORE INTO IndustryJobs VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, %1)").arg(key->ID()));


  q.addBindValue(jobIDs);
  q.addBindValue(activityIDs);
  q.addBindValue(statuses);
  q.addBindValue(runs);
  q.addBindValue(licensedRuns);
  q.addBindValue(outputLocations);
  q.addBindValue(installerIDs);
  q.addBindValue(installerNames);
  q.addBindValue(facilityIDs);
  q.addBindValue(blueprintIDs);
  q.addBindValue(blueprintTypeIDs);
  q.addBindValue(probabilities);
  q.addBindValue(startDates);
  q.addBindValue(endDates);
  q.addBindValue(completedDates);

  if (!q.execBatch())
    qWarning() << q.lastError();


  const QString updateKeyQuery = "UPDATE OR IGNORE CacheTimes "
                                 "SET industryJobs=:Time "
                                 "WHERE keyID=:keyID";

  const QString insertKeyQuery = "INSERT OR IGNORE INTO CacheTimes (keyID, industryJobs) "
                                 "VALUES(:keyID, :Time) ";

  q.prepare(updateKeyQuery);
  q.bindValue(":Time", cachedUntil.toString(Qt::ISODate));
  q.bindValue(":keyID", key->ID());

  if (!q.exec())
    qWarning() << "Update bp cache time error -" << q.lastError();

  q.prepare(insertKeyQuery);
  q.bindValue(":Time", cachedUntil.toString(Qt::ISODate));
  q.bindValue(":keyID", key->ID());

  if (!db.commit()) {
    qWarning() << db.lastError();
    db.rollback();
  }
}

} // namespace eve
