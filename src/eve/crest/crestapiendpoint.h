#ifndef CRESTAPIENDPOINT_H
#define CRESTAPIENDPOINT_H

#include "../crestapirequest.h"

namespace eve {
namespace crest {

class CrestApiEndpoint
{
  public:
    CrestApiEndpoint(const QString& path) : path_(path) {}
    virtual ~CrestApiEndpoint() {}

    CrestApiRequest* toRequest(QSharedPointer<Parser> p) const {
      CrestApiRequest* req = new CrestApiRequest(path_, p);
      return req;
    }


    QString url() const {
      return path_;
    }
  protected:
    QString path_;
};

}
}

#endif // CRESTAPIENDPOINT_H
