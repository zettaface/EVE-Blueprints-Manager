#ifndef ASSETXMLPARSER_H
#define ASSETXMLPARSER_H

#include "xmlparser.h"

namespace eve {

class AssetXmlParser : public XmlParser
{
    Q_OBJECT
  protected:
    void parseInternal(const QByteArray& xml, ApiKeyInfo* key) override;
  signals:
    void finished(ApiKeyInfo*);
};

} //namespace eve

#endif // ASSETXMLPARSER_H
