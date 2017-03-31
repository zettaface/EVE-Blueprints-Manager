#ifndef CREST_HISTORY_H
#define CREST_HISTORY_H

#include "../crestapiendpoint.h"

namespace eve {
namespace crest {

class HistoryRequest : public CrestApiEndpoint
{
  public:
    HistoryRequest(ulong typeID, const QString& path);
};

} //namespace crest
} //namespace eve

#endif // HISTORY_H
