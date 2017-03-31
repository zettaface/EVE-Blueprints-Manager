#ifndef IMAGESERVERREQUEST_H
#define IMAGESERVERREQUEST_H

#include <QSharedPointer>

#include "request.h"
#include "parser/parser.h"

namespace eve {

class ImageServerRequest : public Request
{
  public:
    ImageServerRequest(const QString& type, int ID, int width, QSharedPointer<Parser> parse);

    QString url() const override;
    QString path() const override;
    void parse(QByteArray& data) override;
    long id() const;

  private:
    long id_;
    QString url_;
    const QString host_ {"http://imageserver.eveonline.com/"};
    QStringRef path_;
    QSharedPointer<Parser> parser_;
};

}

#endif // IMAGESERVERREQUEST_H
