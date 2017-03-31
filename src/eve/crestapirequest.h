#ifndef CRESTAPIREQUEST_H
#define CRESTAPIREQUEST_H

#include <QSharedPointer>

#include "request.h"
#include "parser/parser.h"

namespace eve {
namespace crest {

class CrestApiRequest : public Request
{
  public:
    CrestApiRequest(const QString& url, QSharedPointer<Parser> p);
    void parse(QByteArray& data) override;
    QString url() const override;

  private:
    QString url_;
    QSharedPointer<Parser> parser_;
};

} //namespace crest
} //namespace eve

#endif // CRESTAPIREQUEST_H
