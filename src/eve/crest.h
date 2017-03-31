#ifndef CREST_H
#define CREST_H

#include "crestapirequest.h"

#include "crest/market.h"
#include "crest/inventory.h"

namespace eve {

class CrestApi {
  public:
    static crest::MarketRequest market(ulong region) {
      crest::MarketRequest marketRequest(region, host_);
      return marketRequest;
    }

    static crest::InventoryRequest inventory() {
      crest::InventoryRequest inventoryRequest(host_);
      return inventoryRequest;
    }
  private:
    static const QString host_ ;
};

}

#endif // CREST_H
