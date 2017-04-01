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
#include "bpview.h"

class BlueprintsWidget : public QWidget
{
    Q_OBJECT
  public:
    explicit BlueprintsWidget(QWidget* parent = 0);
    ~BlueprintsWidget();

    BlueprintList* model() const { return blueprintsModel; }
    BlueprintListColorizeProxyModel* colorModel() { return bpColorizer; }

    void clearCache();
    QString aggregationInfo() const;

  signals:
    void modelUpdated();

  private:
    QWidget* makeBpConfigWidget();
    QGroupBox* makeOptionsBox();
    QGroupBox* makeSortBox();
    QGroupBox* makeFiltersBox();
    QVBoxLayout* makeOptionsBoxesLayout(QGroupBox* sortGroupBox, QGroupBox* groupingBox, QGroupBox* optionsGroupBox);
    QPushButton* makeMinimizeBpConfigButton(QWidget* bpConfigCont);
    QGroupBox* makeBlueprintsGroupingBox();
    BpView* setupBlueprintsView();

    QMenu* makeLoadFiltersMenu() const;
    void makeLoadFiltersMenu(QDir dir, QMenu* root) const;

    QAbstractItemModel* makeBlueprintsListModel();
    void setBPColumn(int column, int state);
    void installBpProxy(QAbstractProxyModel* model);

    QTreeView* blueprintsView;
    TabWidget* tabWidget;

    QMap<int, QCheckBox*> groupingCheckBoxes;

    BlueprintList* blueprintsModel { nullptr };
    BlueprintListColorizeProxyModel* bpColorizer { nullptr };
    BlueprintsFilterProxy* bpFilterProxy_ { nullptr };
    BlueprintsEmptyGroupsFilter* bpEmptyGroupsProxy_ { nullptr };
    BlueprintsDecorationProxy* bpDecorationsProxy { nullptr };
    BlueprintsAggregationProxy* bpAggregationProxy_ { nullptr };
    BlueprintsSortProxy* bpSortProxy_ { nullptr };
    BlueprintsColumnFilter* bpColumnFilter_ { nullptr };

    QAbstractProxyModel* bpLastModel_ { nullptr };
    BlueprintsFilterBoxWidget* filterBox;

  private slots:
    void setBpModelColorized(bool colorized);
    void sortBlueprints(int);
    void saveFilter();
    void loadFilter(const QString& filename);
    void onBpTabRequest(const QModelIndex& index);
    void onTabClose(int index);
};

#endif // BLUEPRINTSWIDGET_H
