#ifndef INVENTORYTYPEIMAGEPARSER_H
#define INVENTORYTYPEIMAGEPARSER_H

#include "parser.h"

namespace eve {

class InventoryTypeImageParser : public Parser
{
    Q_OBJECT
  protected:
    void parse(QByteArray& raw, Request* req) override;
};

} // namespace eve

#endif // INVENTORYTYPEIMAGEPARSER_H
