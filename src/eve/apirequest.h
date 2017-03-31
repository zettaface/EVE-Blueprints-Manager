#ifndef APIREQUEST_H
#define APIREQUEST_H

#include <QSharedPointer>
#include <QVariant>

#include "apikey.h"
#include "request.h"
#include "parser/parser.h"

namespace eve {

class ApiRequest : public Request
{
    struct ReqParam
    {
      QString parameter;
      QVariant value;
    };

  public:
    ApiRequest(const QString& endpoint, ApiKeyInfo* key_, QSharedPointer<Parser> parse, const QList<ReqParam>& params = {});
    ApiRequest(const QString& endpoint, const QString& callGroup, ApiKeyInfo* key_, QSharedPointer<Parser> parse, const QList<ReqParam>& params = {});

    void parse(QByteArray& data) override;
    QString path() const override;
    QString query() const override;
    QString url() const override;
    ApiKeyInfo* key() const;

  private:
    QString makeQuery(const QList<ReqParam>& params) const;
    ApiKeyInfo* key_ {nullptr};
    QString url_;
    QString path_;
    QString query_;
    QSharedPointer<Parser> parser_;
    const QString host { "https://api.eveonline.com/" };
};

} // namespace eve

#endif // APIREQUEST_H
