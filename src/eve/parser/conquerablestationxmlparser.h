#ifndef CONQUERABLESTATIONXMLPARSER_H
#define CONQUERABLESTATIONXMLPARSER_H

#include "xmlparser.h"

namespace eve {

class ConquerableStationXmlParser : public XmlParser
{
    Q_OBJECT
  protected:
    void parseInternal(const QByteArray& xml, ApiKeyInfo* key) override;
};

} // namespace eve

#endif // CONQUERABLESTATIONXMLPARSER_H
