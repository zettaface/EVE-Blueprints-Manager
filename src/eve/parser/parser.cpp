#include "parser.h"

#include <QtXml>
#include <QtSql>

#include "../api.h"
#include "../apirequest.h"

namespace eve {

Parser::Parser()
{

}

QString Parser::errorString() const
{
  return errorString_;
}

int Parser::error() const
{
  return errorCode_;
}


} // namespace eve
