#ifndef CREST_GROUPS_H
#define CREST_GROUPS_H

#include "../crestapiendpoint.h"

namespace eve {
namespace crest {

class GroupsRequest : public CrestApiEndpoint
{
  public:
    GroupsRequest(const QString& path) :
      CrestApiEndpoint(path) {
      path_.append("groups/");
    }

    GroupsRequest(ulong groupID, const QString& path) :
      CrestApiEndpoint(path) {
      path_.append(QString("groups/%1/").arg(groupID));
    }
};

} //namespace crest
} //namespace eve


#endif // GROUPS_H
