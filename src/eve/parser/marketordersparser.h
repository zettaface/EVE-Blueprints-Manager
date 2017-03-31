#ifndef MARKETORDERSPARSER_H
#define MARKETORDERSPARSER_H

#include "parser.h"
#include "../../priceinfo.h"

namespace eve {

class MarketOrdersParser : public Parser
{
    Q_OBJECT
  public:
    MarketOrdersParser(PriceInfo* price);
    void parse(QByteArray& data, Request* req) override;

  private:
    PriceInfo* price_;
    long stationID {-1};
};

} //namespace eve

#endif // MARKETORDERSPARSER_H
