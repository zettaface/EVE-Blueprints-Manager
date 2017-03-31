#include "marketordersparser.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace eve {

MarketOrdersParser::MarketOrdersParser(PriceInfo* price) :
  price_(price)
{

}


void MarketOrdersParser::parse(QByteArray& data, eve::Request* req)
{
  if (price_ == nullptr)
    return;

  QJsonDocument doc = QJsonDocument::fromJson(data);
  const QJsonArray& items = doc.object()["items"].toArray();

  for (const QJsonValue& i : items) {
    QJsonObject item = i.toObject();

    if (stationID > 0 && item["location"].toObject()["id"].toInt() != stationID)
      continue;

    PriceInfo::Order order;

    order.price = item["price"].toDouble();
    order.quantity = item["quantity"].toInt();

    if (item["buy"].toBool())
      price_->addBuyOrder(order);
    else
      price_->addSellOrder(order);
  }

  price_->updatePrices();

}

} // namespace eve
