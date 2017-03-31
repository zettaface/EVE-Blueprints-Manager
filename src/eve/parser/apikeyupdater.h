#ifndef KEYSTATUSPARSER_H
#define KEYSTATUSPARSER_H

#include "xmlparser.h"

namespace eve {

class ApiKeyUpdater : public XmlParser
{
    Q_OBJECT
  public:
    void parseInternal(const QByteArray& xml, ApiKeyInfo* key) override;

    struct Error
    {
      ApiKeyInfo* key;
      int errorCode;
      QString errorInfo;
    };

    void clearExpiredKeys();
    const QVector<Error>& expiredKeys() const;

  private:
    QVector<Error> expiredKeys_;
};

}

#endif // KEYSTATUSPARSER_H
