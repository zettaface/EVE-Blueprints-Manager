#ifndef CREST_ORDERS_H
#define CREST_ORDERS_H

#include "../crestapiendpoint.h"

namespace eve {
namespace crest {

class OrdersRequest : public CrestApiEndpoint
{
  public:
    OrdersRequest(ulong typeID, const QString& orderType, const QString& path);
    OrdersRequest(ulong typeID, const QString& path);
    OrdersRequest(const QString& path);
};

} //namespace crest
} //namespace eve


#endif // ORDERS_H
