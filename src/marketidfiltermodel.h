#ifndef MARKETIDFILTERMODEL_H
#define MARKETIDFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "blueprintlist.h"

/*
 *  Should be installed only after BlueprintList/Colorizer Proxy/any non filter proxy
*/

class MarketIDFilterModel : public QSortFilterProxyModel
{
  public:
    MarketIDFilterModel(QObject* parent = 0);
    void setBPList(BlueprintList*);
  protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

    int _marketID;
    BlueprintList* _sourceModel;

  public slots:
    void setMarketID(int);
};

#endif // MARKETIDFILTERMODEL_H
