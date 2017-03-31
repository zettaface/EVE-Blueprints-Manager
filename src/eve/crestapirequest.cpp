#include "crestapirequest.h"

namespace eve {
namespace crest {

CrestApiRequest::CrestApiRequest(const QString& url, QSharedPointer<Parser> p) :
  Request(), url_(url), parser_(p)
{
}

QString CrestApiRequest::url() const
{
  return url_;
}

void CrestApiRequest::parse(QByteArray& data)
{
  parser_->parse(data, this);
}

} //namepsace crest
} // namespace eve
