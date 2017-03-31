#ifndef BLUEPRINTSFILTERPROXY_H
#define BLUEPRINTSFILTERPROXY_H

#include <QSortFilterProxyModel>

#include "filters/blueprintfilters.h"

class BlueprintsFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    BlueprintsFilterProxy(QObject* parent = nullptr);

    QSharedPointer<BpFilter> filter() const;
    void setFilter(QSharedPointer<BpFilter> filter);

  signals:
    void invalidated();

  protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

  private:
    QSharedPointer<BpFilter> filter_;
};

#endif // BLUEPRINTSFILTERPROXY_H
