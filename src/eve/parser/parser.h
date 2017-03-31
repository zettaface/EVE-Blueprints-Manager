#ifndef EVEAPIPARSER_H
#define EVEAPIPARSER_H

#include <QObject>
#include <QByteArray>

#include "../request.h"


namespace eve {

class Request;

class Parser: public QObject
{
    Q_OBJECT
  public:
    Parser();
    virtual ~Parser() {}

    virtual void parse(QByteArray& xml, Request* req) = 0;

    QString errorString() const;
    int error() const;

  protected:

    int errorCode_ {0};
    QString errorString_;
};

} //namespace eve

#endif // EVEAPIPARSER_H
