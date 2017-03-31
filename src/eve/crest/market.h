#ifndef CREST_MARKET_H
#define CREST_MARKET_H

#include "crestapiendpoint.h"
#include "market/orders.h"
#include "market/history.h"

namespace eve {
namespace crest {

class MarketRequest : public CrestApiEndpoint
{
  public:
    MarketRequest(ulong region, const QString& path) :
      CrestApiEndpoint(path) {
      path_.append(QString("market/%1/").arg(region));
    }

    HistoryRequest history(ulong typeID) {
      return HistoryRequest(typeID, path_);
    }

    OrdersRequest orders(ulong typeID, const QString& orderType) {
      return OrdersRequest(typeID, orderType, path_);
    }

    OrdersRequest orders(ulong typeID) {
      return OrdersRequest(typeID, path_);
    }

    OrdersRequest orders() {
      return OrdersRequest(path_);
    }
};

} //namespace crest
} //namespace eve

#endif // MARKET_H
