#ifndef EVEAPIKEY_H
#define EVEAPIKEY_H

#include <QDateTime>
#include <QString>
#include <QList>
#include <QObject>
#include <QXmlStreamAttributes>
#include <QSqlRecord>

//TODO private all this stuff
namespace eve {

class ApiKeyInfo
{
  public:
    ApiKeyInfo() = default;
    ApiKeyInfo(const QString& key, const QString& vCode, const QString& type);
    ApiKeyInfo(const QSqlRecord& record);

    QDateTime assetCachedUntil_;
    QDateTime blueprintsCachedUntil_;
    QDateTime industryJobsCachedUntil_;
    QString name() const;

    int ID() const                      { return ID_; }
    quint64 accessMask() const          { return accessMask_; }
    const QString& type() const         { return type_; }
    const QString& vCode() const        { return vCode_; }
    const QString& keyID() const        { return keyID_; }
    const QString& charID() const       { return charID_; }
    const QString& charName() const     { return charName_; }
    const QString& corpID() const       { return corpID_; }
    const QString& corpName() const     { return corpName_; }
    const QString& allianceID() const   { return allianceID_; }
    const QString& allianceName() const { return allianceName_; }

    void setID(int value)                      { ID_ = value; }
    void setAccessMask(const quint64& value)   { accessMask_ = value; }
    void setType(const QString& value)         { type_ = value; }
    void setVCode(const QString& value)        { vCode_ = value; }
    void setKeyID(const QString& value)        { keyID_ = value; }
    void setCharID(const QString& value)       { charID_ = value; }
    void setCharName(const QString& value)     { charName_ = value; }
    void setCorpID(const QString& value)       { corpID_ = value; }
    void setCorpName(const QString& value)     { corpName_ = value; }
    void setAllianceID(const QString& value)   { allianceID_ = value; }
    void setAllianceName(const QString& value) { allianceName_ = value; }

    bool isBlueprintsExpired() const;
    bool isAssetsExpired() const;
    bool isIndustryJobsExpired() const;

    friend bool operator==(const ApiKeyInfo&, const ApiKeyInfo&);
    QString getHost() const;
    void initFromXmlAttributes(const QXmlStreamAttributes& attributes);

    enum Access : quint64
    {
      AssetList = 2,
      Sheet = 8,
      Locations = 16777216
    };
  private:
    int ID_ {-1};
    QString vCode_;
    QString keyID_;
    QString charID_;
    QString charName_;
    QString corpID_;
    QString corpName_;
    QString allianceID_;
    QString allianceName_;
    QString type_;
    quint64 accessMask_;
};

bool operator==(const ApiKeyInfo&, const ApiKeyInfo&);

} // namespace eve

Q_DECLARE_METATYPE(eve::ApiKeyInfo*)
Q_DECLARE_METATYPE(QList<eve::ApiKeyInfo*>)

#endif // EVEAPIKEY_H
