#include "apikeyupdater.h"

#include <QtXml>
#include <QtSql>

namespace eve {

void ApiKeyUpdater::parseInternal(const QByteArray& xml, ApiKeyInfo* key)
{
  if (xml.isEmpty())
    return;

  QXmlStreamReader reader(xml);
  QString type;
  QString accessMask;

  QSqlDatabase db = QSqlDatabase::database(":memory:");

  while (!reader.atEnd() && !reader.hasError()) {
    reader.readNext();

    if (reader.isStartElement()) {
      if (reader.name() == "error") {
        Error e;
        e.key = key;
        e.errorCode = reader.attributes().value("code").toInt();
        e.errorInfo = reader.readElementText();

        expiredKeys_.push_back(e);
        return;
      }

      if (reader.name() == "key") {
        QXmlStreamAttributes attr = reader.attributes();
        type = attr.value("type").toString();
        if (type == "Account" || type == "Character")
          type = "char";
        else
          type = "corp";

        accessMask = attr.value("accessMask").toString();
      }

      if (reader.name() == "row") {
        QXmlStreamAttributes attr = reader.attributes();

        QSqlQuery q(db);

        if (key->charID() == attr.value("characterID").toString()) {
          key->setType(type);
          key->initFromXmlAttributes(attr);
          key->setAccessMask(accessMask.toULongLong());

          q.prepare("UPDATE OR IGNORE ApiKeys "
                    "SET "
                    "  type=:Type, "
                    "  charName=:CharName, "
                    "  corpID=:CorpID, "
                    "  corpName=:CorpName, "
                    "  allianceID=:AllianceID, "
                    "  allianceName=:AllianceName, "
                    "  accessmask=:AccessMask "
                    "WHERE id=:ID");

          q.bindValue(":Type", key->type());
          q.bindValue(":CharName", key->charName());
          q.bindValue(":CorpID", key->corpID());
          q.bindValue(":CorpName", key->corpName());
          q.bindValue(":AllianceID", key->allianceID());
          q.bindValue(":AllianceName", key->allianceName());
          q.bindValue(":AccessMask", key->accessMask());
          q.bindValue(":ID", key->ID());

          if (!q.exec())
            qWarning() << q.lastError();
        }
      }
    }
  }
}

void ApiKeyUpdater::clearExpiredKeys()
{
  expiredKeys_.clear();
}

const QVector<ApiKeyUpdater::Error>& ApiKeyUpdater::expiredKeys() const
{
  return expiredKeys_;
}

}
