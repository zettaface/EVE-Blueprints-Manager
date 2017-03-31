#include "keysrequest.h"

#include <QStringBuilder>

namespace eve {

KeysRequest::KeysRequest(const QString& key, const QString& vCode, QSharedPointer<eve::Parser> parser) :
  Request(), parser_(parser), key_(key), vCode_(vCode)
{
  url_ = host_ % "account/apikeyinfo.xml.aspx?keyID=" % key % "&vCode=" % vCode;
}

QString KeysRequest::url() const
{
  return url_;
}

void KeysRequest::parse(QByteArray& data)
{
  parser_->parse(data, this);
}

QString KeysRequest::key() const
{
  return key_;
}

QString KeysRequest::vCode() const
{
  return vCode_;
}


} // namespace eve
