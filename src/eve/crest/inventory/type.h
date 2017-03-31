#ifndef CREST_TYPE_H
#define CREST_TYPE_H

#include "../crestapiendpoint.h"

namespace eve {
namespace crest {

class TypeRequest : public CrestApiEndpoint
{
  public:
    TypeRequest(const QString& path) :
      CrestApiEndpoint(path) {
      path_.append("types/");
    }

    TypeRequest(ulong typeID, const QString& path) :
      CrestApiEndpoint(path) {
      path_.append(QString("types/%1/").arg(typeID));
    }
};

} //namespace crest
} //namespace eve


#endif // TYPE_H
