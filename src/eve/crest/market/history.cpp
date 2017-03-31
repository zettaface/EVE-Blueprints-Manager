#include "history.h"

#include "../../crest.h"

eve::crest::HistoryRequest::HistoryRequest(ulong typeID, const QString& path) :
  CrestApiEndpoint(path)
{
  path_.append(QString("history/?type=%1").arg(CrestApi::inventory().type(typeID).url()));
}
