#ifndef KEYSREQUEST_H
#define KEYSREQUEST_H

#include <QSharedPointer>

#include "request.h"
#include "parser/parser.h"

namespace eve {

class KeysRequest : public Request
{
  public:
    KeysRequest(const QString& key, const QString& vCode, QSharedPointer<Parser> parse);
    QString url() const override;
    void parse(QByteArray& data) override;
    QString key() const;

    QString vCode() const;

  protected:
    QSharedPointer<Parser> parser_;
    QString url_;
    const QString key_;
    const QString vCode_;
    const QString host_ { "https://api.eveonline.com/" };
};

} // namespace request

#endif // KEYSREQUEST_H
