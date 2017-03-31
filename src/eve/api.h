#ifndef EVEAPI_H
#define EVEAPI_H

#include <QObject>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QEventLoop>

#include "apikey.h"
#include "apikeylist.h"
#include "downloadmanager.h"
#include "parser/parsers.h"
#include "../priceinfo.h"

namespace eve {

class Api : public QObject
{
    Q_OBJECT
  public:
    explicit Api(QObject* parent = 0);
    Api(const Api&) = delete;

    ~Api();

    void init();

    QDateTime nextAssetsFetchTime() const { return keys_->nextAssetsFetchTime(); }
    QDateTime nextBlueprintsFetchTime() const { return keys_->nextBlueprintsFetchTime(); }
    QDateTime nextIndustryJobsFetchTime() const { return keys_->nextIndustryJobsFetchTime(); }

    void prepareMemoryDB();
    void fetchKeys(const QString& key, const QString& vcode);
    void fetchKeys(const QString& key, const QString& vcode, QSharedPointer<Parser> parser);

    void saveKey(ApiKeyInfo* key);
    void removeKey(ApiKeyInfo* key);
    void removeKeyById(int keyID);
    ApiKeyList* keys() const;

    PriceInfo* priceInfo(long typeID);

    QString currentFetchRequest() const;

    QPixmap typeIcon(long typeID) const;

    const QVector<ApiKeyUpdater::Error>& keyCheckErrors() const;

  public slots:
    void clearCache();
    void updateApiKeys();
    void updateApiKeys(QSharedPointer<Parser> parser);
    int queueAssetsFetch();
    int queueAssetsFetch(QSharedPointer<XmlParser> parser);
    int queueBlueprintsFetch();
    int queueBlueprintsFetch(QSharedPointer<XmlParser> parser);
    int queueIndustryJobsFetch();
    int queueIndustryJobsFetch(QSharedPointer<XmlParser> parser);
    void queueConqStationsFetch();
    void queueTypeIconFetch(long typeID);
    void queueAllTypeIconsFetch();
    int queueLocationsFetch(ApiKeyInfo* key);
    int queueLocationsFetch(ApiKeyInfo* key, QSharedPointer<XmlParser> parser);
  private:
    void showExpiredKeysDialog() const;
    QHash<long, PriceInfo*> prices_;
    DownloadManager downloadManager;
    ApiKeyList* keys_;

    QSharedPointer<ApiKeyUpdater> keyStatusParser;
    int keysToCheck_ = 0;
    int keysChecked_ = 0;
  private slots:
    void updatePrices(QVector<PriceInfo*>);
    void proceedKeysChecking();
  signals:
    void fetchFinished();
    void keysFetched(QList<ApiKeyInfo*>);

    void processing(int, int);

    void locationRequestsAdded(int);

    void assetReqParsed();
    void blueprintReqParsed();
    void locationReqParsed();
    void industryJobReqParsed();
    void conqStationsReqParsed();

    void keysChecked(bool);
};

extern Api API;

}

#endif // EVEAPI_H
