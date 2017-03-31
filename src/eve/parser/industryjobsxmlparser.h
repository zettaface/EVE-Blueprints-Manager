#ifndef INDUSTRYJOBSXMLPARSER_H
#define INDUSTRYJOBSXMLPARSER_H

#include "xmlparser.h"

namespace eve {

class IndustryJobsXmlParser : public XmlParser
{
    Q_OBJECT
  protected:
    void parseInternal(const QByteArray& xml, ApiKeyInfo* key) override;
};

} // namespace eve

#endif // INDUSTRYJOBSXMLPARSER_H
