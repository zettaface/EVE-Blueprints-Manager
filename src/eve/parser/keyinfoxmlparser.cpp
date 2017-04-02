#include "keyinfoxmlparser.h"

#include <QtXml>
#include <QtSql>
#include <QDebug>

#include "../keysrequest.h"

namespace eve {

void KeyInfoXmlParser::parse(QByteArray& xml, Request* req)
{
  if (xml.isEmpty())
    return;

  QXmlStreamReader reader(xml);
  QString type;
  QString accessMask;

  while (!reader.atEnd() && !reader.hasError()) {
    reader.readNext();

    if (reader.isStartDocument())
      continue;

    if (reader.isStartElement()) {
      if (reader.name() == "error") {
        errorCode_ = reader.attributes().value("code").toInt();
        errorString_ = reader.readElementText();
        return;
      }
      if (reader.name() == "key") {
        QXmlStreamAttributes attr = reader.attributes();
        type = attr.value("type").toString();
        if (type == "Account" || type == "Character")
          type = "char";
        else
          type = "corp";

        accessMask = attr.value("accessMask").toString();
      }
      if (reader.name() == "row") {
        QXmlStreamAttributes attr = reader.attributes();
        KeysRequest* request = static_cast<KeysRequest*>(req);

        ApiKeyInfo* keyinfo = new ApiKeyInfo(request->key(), request->vCode(), type);
        keyinfo->initFromXmlAttributes(attr);
        keyinfo->setAccessMask(accessMask.toULongLong());

        keys_.append(keyinfo);
      }
    }
  }

  if (reader.hasError()) {
    for (auto key : keys_)
      delete key;

    qWarning() << reader.errorString();
    keys_.clear();
  }

  emit finished(keys_);
}

} // namespace eve
