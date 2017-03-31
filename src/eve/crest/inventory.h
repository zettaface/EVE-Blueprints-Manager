#ifndef CREST_INVENTORY_H
#define CREST_INVENTORY_H

#include "crestapiendpoint.h"
#include "inventory/type.h"
#include "inventory/groups.h"

namespace eve {
namespace crest {

class InventoryRequest : public CrestApiEndpoint
{
  public:
    InventoryRequest(const QString& path) :
      CrestApiEndpoint(path) {
      path_.append("inventory/");
    }

    TypeRequest types() {
      return TypeRequest(path_);
    }

    TypeRequest type(ulong type) {
      return TypeRequest(type, path_);
    }

    GroupsRequest groups() {
      return GroupsRequest(path_);
    }

    GroupsRequest group(ulong group) {
      return GroupsRequest(group, path_);
    }
};

}
}

#endif // INVENTORY_H
