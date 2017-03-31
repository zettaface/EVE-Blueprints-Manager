#ifndef BLUEPRINTSXMLPARSER_H
#define BLUEPRINTSXMLPARSER_H

#include "xmlparser.h"

namespace eve {

class BlueprintsXmlParser : public XmlParser
{
    Q_OBJECT
  protected:
    void parseInternal(const QByteArray& xml, ApiKeyInfo* key) override;
};

} // namespace eve

#endif // BLUEPRINTSXMLPARSER_H
