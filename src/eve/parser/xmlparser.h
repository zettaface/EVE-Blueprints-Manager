#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "parser.h"
#include "../apikey.h"

namespace eve {

class XmlParser : public Parser
{
    Q_OBJECT
  public:
    void parse(QByteArray& xml, Request* req) override;

    virtual void parseInternal(const QByteArray& xml, ApiKeyInfo* key) = 0;
  signals:
    void parsed(ApiKeyInfo*);
};

} // namespace eve

#endif // XMLPARSER_H
