#ifndef LOCATIONSXMLPARSER_H
#define LOCATIONSXMLPARSER_H

#include "xmlparser.h"

namespace eve {

class LocationsXmlParser : public XmlParser
{
    Q_OBJECT
  public:
    LocationsXmlParser(bool clearCache = true);

  protected:
    void parseInternal(const QByteArray& xml, ApiKeyInfo* key) override;

  private:
    bool clearCache_;
};

} // namespace eve

#endif // LOCATIONSXMLPARSER_H
