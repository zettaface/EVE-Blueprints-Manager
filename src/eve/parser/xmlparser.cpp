#include "xmlparser.h"

#include "../apirequest.h"

namespace eve {

void XmlParser::parse(QByteArray& xml, Request* req)
{
  ApiRequest* request = static_cast<ApiRequest*>(req);
  parseInternal(xml, request->key());

  emit parsed(request->key());
}

}
