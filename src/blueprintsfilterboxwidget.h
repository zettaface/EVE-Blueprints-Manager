#ifndef BLUEPRINTSFILTERBOXWIDGET_H
#define BLUEPRINTSFILTERBOXWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include "filters/blueprintfilters.h"
#include "blueprintsfilterproxy.h"
#include "blueprintsfilterwidget.h"

class BlueprintsFilterBoxWidget : public QWidget
{
    Q_OBJECT
  public:
    explicit BlueprintsFilterBoxWidget(QWidget* parent = 0);
    explicit BlueprintsFilterBoxWidget(const QSet<int>& filterTypes, QWidget* parent = 0);
    explicit BlueprintsFilterBoxWidget(QSharedPointer<BpComplexFilter> filter, QWidget* parent = 0);
    explicit BlueprintsFilterBoxWidget(const QVector<int>& filterTypes, QSharedPointer<BpComplexFilter> filter, QWidget* parent = 0);
    explicit BlueprintsFilterBoxWidget(const QSet<int>& filterTypes, QSharedPointer<BpComplexFilter> filter, QWidget* parent = 0);
    QSharedPointer<BpComplexFilter> filter() const;
    void setFilter(const QSharedPointer<BpComplexFilter>& filter);

  private:
    void installFilter(QSharedPointer<BpFilter>);
    QColor generateColor(const QColor& ) const;

    QVector<int> filterTypes_;
    QSharedPointer<BpComplexFilter> filter_;
    QVBoxLayout* filtersLayout;

  private slots:
    void addFilter();
    void addFilter(QSharedPointer<BpFilter> filter);
    void addFilterWidget(BlueprintsFilterWidget* bfw);
    void removeFilter(QWidget*, QSharedPointer<BpFilter>);
    void onFilterTypeChange(QSharedPointer<BpFilter> oldFilter, QSharedPointer<BpFilter> newFilter);

};

#endif // BLUEPRINTSFILTERBOXWIDGET_H
