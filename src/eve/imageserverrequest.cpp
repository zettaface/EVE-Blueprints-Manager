#include "imageserverrequest.h"

#include "QStringBuilder"

namespace eve {

ImageServerRequest::ImageServerRequest(const QString& type, int ID, int width, QSharedPointer<Parser> parser) :
  Request(), parser_(parser), id_(ID)
{
  QString path = QString("%1/%2_%3.png").arg(type).arg(ID).arg(width);
  url_ = host_ % path;
  path_ = url_.midRef(host_.size(), path.size());
}

QString ImageServerRequest::url() const
{
  return url_;
}

QString ImageServerRequest::path() const
{
  return path_.toString();
}

long ImageServerRequest::id() const
{
  return id_;
}

void ImageServerRequest::parse(QByteArray& data)
{
  parser_->parse(data, this);
}

}

