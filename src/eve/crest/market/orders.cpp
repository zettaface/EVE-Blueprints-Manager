#include "orders.h"

#include "../../crest.h"

eve::crest::OrdersRequest::OrdersRequest(ulong typeID, const QString& orderType, const QString& path) :
  CrestApiEndpoint(path)
{
  path_.append(QString("orders/%1/?type=%2").arg(orderType, CrestApi::inventory().type(typeID).url()));

}

eve::crest::OrdersRequest::OrdersRequest(ulong typeID, const QString& path) :
  CrestApiEndpoint(path)
{
  path_.append(QString("orders/?type=%1").arg(CrestApi::inventory().type(typeID).url()));
}

eve::crest::OrdersRequest::OrdersRequest(const QString& path) :
  CrestApiEndpoint(path)
{
  path_.append("orders/all/");
}
