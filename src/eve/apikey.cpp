#include "apikey.h"

#include <QVariant>

#include <algorithm>
#include <tuple>

namespace eve {

ApiKeyInfo::ApiKeyInfo(const QString& key, const QString& vCode, const QString& type) :
  keyID_(key), vCode_(vCode), type_(type)
{

}

ApiKeyInfo::ApiKeyInfo(const QSqlRecord& record)
{
  ID_           = record.value("id").toInt();
  keyID_        = record.value("keyID").toString();
  charID_       = record.value("charID").toString();
  vCode_        = record.value("vCode").toString();
  type_         = record.value("type").toString();
  charName_     = record.value("charName").toString();
  corpID_       = record.value("corpID").toString();
  corpName_     = record.value("corpName").toString();
  allianceID_   = record.value("allianceID").toString();
  allianceName_ = record.value("allianceName").toString();
  accessMask_   = record.value("accessMask").toULongLong();
}

void ApiKeyInfo::initFromXmlAttributes(const QXmlStreamAttributes& attributes)
{
  charID_       = attributes.value("characterID").toString();
  charName_     = attributes.value("characterName").toString();
  corpID_       = attributes.value("corporationID").toString();
  corpName_     = attributes.value("corporationName").toString();
  allianceID_   = attributes.value("allianceID").toString();
  allianceName_ = attributes.value("allianceName").toString();
}

QString ApiKeyInfo::name() const
{
  if (type_ == "corp")
    return corpName_;
  else if (type_ == "char")
    return charName_;
  else
    return "";
}

bool ApiKeyInfo::isBlueprintsExpired() const
{
  return blueprintsCachedUntil_ <= QDateTime::currentDateTimeUtc();
}

bool ApiKeyInfo::isAssetsExpired() const
{
  return assetCachedUntil_ <= QDateTime::currentDateTimeUtc();
}

bool ApiKeyInfo::isIndustryJobsExpired() const
{
  return industryJobsCachedUntil_ <= QDateTime::currentDateTimeUtc();
}

bool operator==(const ApiKeyInfo& k1, const ApiKeyInfo& k2)
{
  return std::tie(k1.keyID_, k1.vCode_, k1.charID_) ==
         std::tie(k2.keyID_, k2.vCode_, k2.charID_);
}

} // namespace eve
