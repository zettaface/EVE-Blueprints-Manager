#ifndef BLUEPRINTSWIDGET_H
#define BLUEPRINTSWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QMap>
#include <QMenu>
#include <QTreeView>
#include <QCheckBox>
#include <QGroupBox>

#include "tabwidget.h"
#include "blueprintlist.h"
#include "blueprintlistcolorizeproxymodel.h"
#include "blueprintsfilterproxy.h"
#include "blueprintsfilterwidget.h"
#include "blueprintsaggregationproxy.h"
#include "blueprintssortproxy.h"
#include "blueprintsdecorationproxy.h"
#include "blueprintsemptygroupsfilter.h"
#include "blueprintscolumnfilter.h"
#include "blueprintsfilterboxwidget.h"

class BlueprintsWidget : public QWidget
{
    Q_OBJECT
  public:
    explicit BlueprintsWidget(QWidget* parent = 0);
    ~BlueprintsWidget();

    BlueprintList* model() const { return blueprintsModel; }
    BlueprintListColorizeProxyModel* colorModel() { return bpColorizer; }

    void clearCache() {
      bpColorizer->clearCache();
      bpDecorationsProxy->clearCache();
      bpAggregationProxy_->clearCache();
    }

    QString aggregationInfo() const;

  signals:
    void modelUpdated();

  public slots:
  private:
    QGroupBox* makeBPGroupBox();
    void setBPColumn(int column, int state);
    QMenu* makeLoadFiltersMenu() const;
    void makeLoadFiltersMenu(QDir dir, QMenu* root) const;

    QTreeView* blueprintsView;
    QComboBox* sortComboBox;
    QComboBox* sortOrderComboBox;
    TabWidget* tabWidget;

    QMap<int, QCheckBox*> groupingCheckBoxes;

    BlueprintList* blueprintsModel;
    BlueprintListColorizeProxyModel* bpColorizer;
    BlueprintsFilterProxy* bpFilterProxy_;
    BlueprintsEmptyGroupsFilter* bpEmptyGroupsProxy_;
    BlueprintsDecorationProxy* bpDecorationsProxy;
    BlueprintsAggregationProxy* bpAggregationProxy_;
    BlueprintsSortProxy* bpSortProxy_;
    BlueprintsColumnFilter* bpColumnFilter_;

    QAbstractProxyModel* bpLastModel_ { nullptr };
    QVBoxLayout* filtersLayout;
    BlueprintsFilterBoxWidget* filterBox;
    QAbstractItemModel* makeBlueprintsListModel();
    void installBpProxy(QAbstractProxyModel* model);

  private slots:
    void setBpModelColorized(bool colorized);
    void sortBlueprints(int);
    void saveFilter();
    void loadFilter(const QString& filename);
    void onBpTabRequest(const QModelIndex& index);
    void onTabClose(int index);
};

#endif // BLUEPRINTSWIDGET_H
