#include "apirequest.h"

#include <QStringBuilder>

namespace eve {

ApiRequest::ApiRequest(const QString& endpoint, ApiKeyInfo* key, QSharedPointer<Parser> parser, const QList<ReqParam>& params) :
  ApiRequest(endpoint, key->type(), key, parser, params)
{
}

ApiRequest::ApiRequest(const QString& endpoint, const QString& callGroup, ApiKeyInfo* key, QSharedPointer<Parser> parser, const QList<ApiRequest::ReqParam>& params) :
  Request(), parser_(parser), path_(endpoint), key_(key)
{
  query_ = makeQuery(params);
  url_ = host % callGroup % path_ % query_;
}

void ApiRequest::parse(QByteArray& data)
{
  parser_->parse(data, this);
}

QString ApiRequest::path() const
{
  return path_;
}

QString ApiRequest::query() const
{
  return query_;
}

QString ApiRequest::url() const
{
  return url_;
}

QString ApiRequest::makeQuery(const QList<ApiRequest::ReqParam>& params) const
{
  if (key_ == nullptr)
    return "";

  QString query;
  if (key_->type() == "char" || !key_->charID().isEmpty())
    query = QString("?keyID=%1&vCode=%2&characterID=%3").arg(key_->keyID(), key_->vCode(), key_->charID());
  else if (key_->type() == "api")
    query = "?";
  else if (key_->type() == "eve")
    query = "";
  else if (key_->type() == "corp" || key_->type() == "account")
    query = QString("?keyID=%1&vCode=%2").arg(key_->keyID(), key_->vCode());

  for (const ReqParam& p : params)
    query += QString("&%1=%2").arg(p.parameter, p.value.toString());

  return query;
}

ApiKeyInfo* ApiRequest::key() const
{
  return key_;
}

} //namespace eve
