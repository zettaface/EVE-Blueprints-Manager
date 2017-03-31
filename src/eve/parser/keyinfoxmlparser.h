#ifndef KEYINFOXMLPARSER_H
#define KEYINFOXMLPARSER_H

#include "xmlparser.h"

namespace eve {

class KeyInfoXmlParser : public Parser
{
    Q_OBJECT
  public:
    QList<ApiKeyInfo*> keys() const;
    void clearKeys() { keys_.clear(); }

  protected:
    void parse(QByteArray& xml, Request* req) override;
    QList<ApiKeyInfo*> keys_;

  signals:
    void finished(QList<ApiKeyInfo*>);
};

} // namespace eve

#endif // KEYINFOXMLPARSER_H
