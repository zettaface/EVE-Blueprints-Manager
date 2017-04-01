#include "blueprintswidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

#include <fstream>

#include "blueprintpage.h"
#include "bpview.h"

BlueprintsWidget::BlueprintsWidget(QWidget* parent) : QWidget(parent)
{
  blueprintsView = new BpView(this);
  blueprintsView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  blueprintsView->setSelectionMode(QAbstractItemView::ContiguousSelection);
  blueprintsView->setSelectionBehavior(QAbstractItemView::SelectItems);

  filtersLayout = new QVBoxLayout;
  filtersLayout->setAlignment(Qt::AlignTop);
  filtersLayout->setSpacing(3);


  QGroupBox* filtersGroupBox = new QGroupBox("Filters", this);
  filtersGroupBox->setLayout(filtersLayout);

  QHBoxLayout* bpConfigL = new QHBoxLayout;
  bpConfigL->setMargin(0);

  QCheckBox* flatListCheckBox = new QCheckBox("Flat", this);
  QCheckBox* colorizedCheckBox = new QCheckBox("No color", this);
  QCheckBox* showMissingBpBox = new QCheckBox("Show missing blueprints", this);

  QGroupBox* optionsGroupBox = new QGroupBox("Options", this);
  QVBoxLayout* optionsL = new QVBoxLayout;
  optionsL->addWidget(flatListCheckBox);
  optionsL->addWidget(colorizedCheckBox);
  optionsL->addWidget(showMissingBpBox);
  optionsL->setSpacing(2);
  optionsGroupBox->setLayout(optionsL);

  QGroupBox* sortGroupBox = new QGroupBox("Sort by...", this);
  QVBoxLayout* sortBoxL = new QVBoxLayout;

  sortGroupBox->setLayout(sortBoxL);

  sortComboBox = new QComboBox(this);
  sortOrderComboBox = new QComboBox(this);

  sortBoxL->addWidget(sortComboBox);
  sortBoxL->addWidget(sortOrderComboBox);

  sortComboBox->addItem("Name", BlueprintsSortProxy::Name);
  sortComboBox->addItem("Runs", BlueprintsSortProxy::Runs);
  sortComboBox->addItem("Quantity", BlueprintsSortProxy::Quantity);
  sortComboBox->addItem("Location", BlueprintsSortProxy::Location);
  sortComboBox->addItem("ME", BlueprintsSortProxy::ME);
  sortComboBox->addItem("TE", BlueprintsSortProxy::TE);
  sortComboBox->addItem("Color(experimental)", BlueprintsSortProxy::Color);

  sortOrderComboBox->addItem("Ascending", QVariant::fromValue(Qt::AscendingOrder));
  sortOrderComboBox->addItem("Descending", QVariant::fromValue(Qt::DescendingOrder));

  connect(sortComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sortBlueprints(int)));
  connect(sortOrderComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sortBlueprints(int)));

  QVBoxLayout* leftBlockL = new QVBoxLayout;
  leftBlockL->setMargin(0);
  leftBlockL->setSpacing(1);
  leftBlockL->addWidget(sortGroupBox);
  leftBlockL->addWidget(optionsGroupBox);
  leftBlockL->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum));
  leftBlockL->setStretch(2, 5);

  QVBoxLayout* middleBlockL = new QVBoxLayout;

  middleBlockL->setMargin(0);
  middleBlockL->addWidget(makeBPGroupBox());
//  middleBlockL->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Minimum, QSizePolicy::Expanding));

  bpConfigL->addLayout(leftBlockL);
  bpConfigL->addLayout(middleBlockL);
  bpConfigL->addWidget(filtersGroupBox);

  QWidget* bpConfigCont = new QWidget(this);
  bpConfigCont->setLayout(bpConfigL);
  bpConfigCont->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  QPushButton* minimizeBpConfigBtn = new QPushButton(QChar(0x02C4), this);
  minimizeBpConfigBtn->setCheckable(true);
//  minimizeBpConfigBtn->setFlat(true);
  minimizeBpConfigBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
  connect(minimizeBpConfigBtn, &QPushButton::toggled, [bpConfigCont, minimizeBpConfigBtn](bool checked) {
    bpConfigCont->setVisible(!checked);
    if(checked)
      minimizeBpConfigBtn->setText(QChar(0x02C5));
    else
      minimizeBpConfigBtn->setText(QChar(0x02C4));
  });

  QWidget* bpCont = new QWidget(this);
  QVBoxLayout* bpLayout = new QVBoxLayout(bpCont);
  bpLayout->addWidget(bpConfigCont);
  bpLayout->addWidget(minimizeBpConfigBtn);
  bpLayout->addWidget(blueprintsView);

  bpLayout->setStretch(0, 0);
  bpLayout->setStretch(1, 0);
  bpLayout->setStretch(2, 5);

  blueprintsView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  blueprintsView->setUniformRowHeights(true);
  blueprintsView->setModel(makeBlueprintsListModel());
  blueprintsView->setColumnWidth(Blueprint::Activity, 24);

  tabWidget = new TabWidget(this);
  tabWidget->addTab(bpCont, "Blueprints");
  tabWidget->setTabsClosable(true);
  tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);

  connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabClose(int)));

  QVBoxLayout* l = new QVBoxLayout(this);
  l->setSpacing(0);
  l->setMargin(1);
  l->addWidget(tabWidget);

  using Type = BpFilterFactory::Type;
  QSet<int> filterTypes_t = BpFilterFactory::instance().availableFilters().subtract({Type::IsHeader});

  filterBox = new BlueprintsFilterBoxWidget(filterTypes_t, qSharedPointerCast<BpComplexFilter>(BpFilterFactory::instance().createFilter(BpFilterFactory::Logical)), this);
  bpFilterProxy_->setFilter(filterBox->filter());

  QPushButton* saveFiltersBtn = new QPushButton("Save", this);
  connect(saveFiltersBtn, SIGNAL(clicked(bool)), this, SLOT(saveFilter()));

  QPushButton* loadFilterBtn = new QPushButton("Load", this);
  connect(loadFilterBtn, SIGNAL(clicked(bool)), this, SLOT(loadFilter()));

  QHBoxLayout* filterButtonsL = new QHBoxLayout;
  filterButtonsL->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
  filterButtonsL->addWidget(saveFiltersBtn);
  filterButtonsL->addWidget(loadFilterBtn);
  filterButtonsL->setSpacing(0);
  filterButtonsL->setMargin(1);

  filtersLayout->addWidget(filterBox);
  filtersLayout->addLayout(filterButtonsL);

  connect(flatListCheckBox, SIGNAL(toggled(bool)), blueprintsModel, SLOT(setFlat(bool)));
  connect(flatListCheckBox, SIGNAL(toggled(bool)), bpEmptyGroupsProxy_, SLOT(setDisabled(bool)));
  connect(flatListCheckBox, SIGNAL(toggled(bool)), blueprintsModel, SLOT(loadFromMemory()));
  connect(colorizedCheckBox, SIGNAL(toggled(bool)), this, SLOT(setBpModelColorized(bool)));
  connect(showMissingBpBox, SIGNAL(toggled(bool)), blueprintsModel, SLOT(setShowAll(bool)));
  connect(showMissingBpBox, SIGNAL(toggled(bool)), blueprintsModel, SLOT(loadFromMemory()));
//  connect(blueprintsModel, SIGNAL(modelFetched()), this, SLOT(updateStatusMessage()));
  connect(bpFilterProxy_, SIGNAL(invalidated()), this, SIGNAL(modelUpdated()), Qt::QueuedConnection);
  connect(blueprintsModel, SIGNAL(modelReset()), this, SIGNAL(modelUpdated()));
  connect(blueprintsView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onBpTabRequest(QModelIndex)));

  blueprintsView->resizeColumnToContents(0);
}

BlueprintsWidget::~BlueprintsWidget()
{
  QSettings settings;
  QVariantList filteredColumns;

  for (int column : bpColumnFilter_->filteredColumns())
    filteredColumns << column;

  settings.setValue("mainWindow/filteredColumns", filteredColumns);
}

QString BlueprintsWidget::aggregationInfo() const
{
  QString aggregationInfo("%2(%3 unique) blueprints, %1 runs total");
  aggregationInfo = aggregationInfo.arg(bpAggregationProxy_->totalRuns()).arg(bpAggregationProxy_->totatQuantity()).arg(bpAggregationProxy_->rowCount());

  return aggregationInfo;
}

QGroupBox* BlueprintsWidget::makeBPGroupBox()
{
  QGroupBox* group = new QGroupBox("Group by...", this);

  QCheckBox* nameColumn = new QCheckBox("Name", this);
  QCheckBox* activityColumn = new QCheckBox("Activity", this);
  QCheckBox* runsColumn = new QCheckBox("Runs", this);
  QCheckBox* MEColumn = new QCheckBox("ME", this);
  QCheckBox* TEColumn = new QCheckBox("TE", this);
  QCheckBox* locationColumn = new QCheckBox("Location", this);
  QCheckBox* ownerColumn = new QCheckBox("Owner", this);

  nameColumn->setChecked(true);
  nameColumn->setEnabled(false);

  groupingCheckBoxes = QMap<int, QCheckBox*> {
    {Blueprint::Name, nameColumn},
    {Blueprint::Activity, activityColumn},
    {Blueprint::Runs, runsColumn},
    {Blueprint::MEc, MEColumn},
    {Blueprint::TEc, TEColumn},
    {Blueprint::LocationIDc, locationColumn},
    {Blueprint::Owner, ownerColumn}
  };

  QList<int> columnList = groupingCheckBoxes.keys();

  QSettings settings;
  QVariantList columnsList = settings.value("mainWindow/filteredColumns", QVariantList{Blueprint::MetaLevel}).toList();
  QSet<int> filteredColumns;

  for (QVariant column : columnsList)
    filteredColumns.insert(column.toInt());

  for (int column : columnList) {
    groupingCheckBoxes[column]->setChecked(!filteredColumns.contains(column));
    connect(groupingCheckBoxes[column], &QCheckBox::stateChanged, [this, column](int state) {
      setBPColumn(column, state);
    });
  }

  QVBoxLayout* boxLayout = new QVBoxLayout(group);
  boxLayout->addWidget(nameColumn);
  boxLayout->addWidget(activityColumn);
  boxLayout->addWidget(runsColumn);
  boxLayout->addWidget(MEColumn);
  boxLayout->addWidget(TEColumn);
  boxLayout->addWidget(locationColumn);
  boxLayout->addWidget(ownerColumn);
  boxLayout->setSpacing(2);

  group->setLayout(boxLayout);

  return group;
}

void BlueprintsWidget::setBPColumn(int column, int state)
{
  bool isColumnHidden = state == Qt::Checked ? false : true;

  qDebug() << column;
  if (isColumnHidden)
    blueprintsModel->removeGroupColumn(column);
  else
    blueprintsModel->addGroupColumn(column);

  blueprintsModel->loadFromMemory();
  bpColumnFilter_->filterColumn(column, isColumnHidden);

  if (column > 0)
    blueprintsView->resizeColumnToContents(column - 1);

  blueprintsView->resizeColumnToContents(column);
}

QAbstractItemModel* BlueprintsWidget::makeBlueprintsListModel()
{
  QSettings settings;
  QVariantList columnsList = settings.value("mainWindow/filteredColumns", QVariantList{Blueprint::MetaLevel}).toList();
  QSet<int> filteredColumns;

  for (QVariant column : columnsList)
    filteredColumns.insert(column.toInt());

  blueprintsModel = new BlueprintList(this);
  bpColorizer = new BlueprintListColorizeProxyModel(this);
  bpFilterProxy_ = new BlueprintsFilterProxy(this);
  bpEmptyGroupsProxy_ = new BlueprintsEmptyGroupsFilter(this);
  bpDecorationsProxy = new BlueprintsDecorationProxy(this);
  bpAggregationProxy_ = new BlueprintsAggregationProxy(this);
  bpSortProxy_ = new BlueprintsSortProxy(this);
  bpColumnFilter_ = new BlueprintsColumnFilter(filteredColumns, this);

  installBpProxy(bpColorizer);
  installBpProxy(bpFilterProxy_);
  installBpProxy(bpEmptyGroupsProxy_);
  installBpProxy(bpDecorationsProxy);
  installBpProxy(bpAggregationProxy_);
  installBpProxy(bpSortProxy_);
  installBpProxy(bpColumnFilter_);

  QSet<int> allColumns {
    Blueprint::Name,
    Blueprint::Runs,
    Blueprint::MEc,
    Blueprint::TEc,
    Blueprint::Activity,
    Blueprint::LocationIDc,
    Blueprint::Owner
  };

  blueprintsModel->setGroupColumns(allColumns.subtract(filteredColumns));
  blueprintsModel->loadFromMemory();

  connect(blueprintsModel, SIGNAL(modelFetched()), bpFilterProxy_, SLOT(invalidate()));
  connect(blueprintsModel, SIGNAL(modelFetched()), bpAggregationProxy_, SLOT(clearCache()));
  connect(blueprintsModel, SIGNAL(modelFetched()), bpDecorationsProxy, SLOT(clearCache()));
  connect(blueprintsModel, SIGNAL(modelFetched()), bpColorizer, SLOT(clearCache()));

  connect(bpFilterProxy_, SIGNAL(invalidated()), bpEmptyGroupsProxy_, SLOT(invalidate()), Qt::QueuedConnection);
  connect(bpFilterProxy_, SIGNAL(invalidated()), bpAggregationProxy_, SLOT(clearCache()), Qt::QueuedConnection);
  connect(bpFilterProxy_, SIGNAL(invalidated()), bpDecorationsProxy, SLOT(clearCache()), Qt::QueuedConnection);

  return bpLastModel_;
}

void BlueprintsWidget::installBpProxy(QAbstractProxyModel* model)
{
  if (bpLastModel_ != nullptr)
    model->setSourceModel(bpLastModel_);
  else
    model->setSourceModel(blueprintsModel);

  bpLastModel_ = model;
}

void BlueprintsWidget::setBpModelColorized(bool colorized)
{
  if (colorized) {
    bpFilterProxy_->setSourceModel(blueprintsModel);
    bpAggregationProxy_->setSourceModel(bpEmptyGroupsProxy_);
  } else {
    bpFilterProxy_->setSourceModel(bpColorizer);
    bpAggregationProxy_->setSourceModel(bpDecorationsProxy);
  }
}

void BlueprintsWidget::sortBlueprints(int)
{
  typedef Blueprint::Columns Columns;
  typedef BlueprintsSortProxy::Type Type;
  int selectedType = sortComboBox->currentData().toInt();
  Qt::SortOrder order = sortOrderComboBox->currentData().value<Qt::SortOrder>();

  bpSortProxy_->setType(selectedType);

  switch(selectedType) {
    case Type::Name:

      bpSortProxy_->sort(Columns::Name, order);
      break;
    case Type::Location:
      bpSortProxy_->sort(Columns::LocationIDc, order);
      break;
    case Type::Runs:
      bpSortProxy_->sort(Columns::Runs, order);
      break;
    case Type::Quantity:
      bpSortProxy_->sort(Columns::Quantity, order);
      break;
    case Type::ME:
      bpSortProxy_->sort(Columns::MEc, order);
      break;
    case Type::TE:
      bpSortProxy_->sort(Columns::TEc, order);
      break;
    case Type::Color:
      bpSortProxy_->sort(Columns::Name, order);
      break;
    default:
      qDebug() << "Unkonwn sort column. Got " << selectedType;
      break;
  }
}

void BlueprintsWidget::saveFilter()
{
  BpFilterFactory factory;
  QString fileName = QFileDialog::getSaveFileName(this, "Save filter", "", "YAML(*.yaml)");

  if (fileName.isEmpty())
    return;

  std::ofstream yOut(QString("%1.yaml").arg(fileName.split(".").at(0)).toStdString());
  yOut << factory.saveFiltersAsYaml(filterBox->filter());
}

void BlueprintsWidget::loadFilter()
{
  BpFilterFactory factory;
  QString fileName = QFileDialog::getOpenFileName(this, "Open filter", "", "YAML(*.yaml)");

  if (fileName.isEmpty())
    return;

  QSharedPointer<BpFilter> ptr = factory.loadFilters(YAML::LoadFile(fileName.toStdString()));

  if (ptr.isNull())
    qWarning() << "NULL FILTER POINTER";
  else {
    filterBox->setFilter(qSharedPointerCast<BpComplexFilter>(ptr));
    bpFilterProxy_->setFilter(ptr);
  }
}

void BlueprintsWidget::onBpTabRequest(const QModelIndex& index)
{
  if (index.data(BlueprintList::IsHeaderRole).toBool()) {
    qDebug() << "Header clicked";
    return;
  }

  Blueprint bp = index.data(BlueprintList::BlueprintRole).value<Blueprint>();

  if (bp.prodTypeID <= 0 || bp.typeID <= 0) {
    QMessageBox::warning(this,
                         "Blueprint not found",
                         "Program cannot load blueprint information.<br><br>"
                         "Maybe static data too old?");

    return;
  }

  int tabID = tabWidget->addTab(new BlueprintPage(bp, this),
                                bp.displayData(Blueprint::Name).toString());

  tabWidget->setCurrentIndex(tabID);
}

void BlueprintsWidget::onTabClose(int index)
{
  QWidget* tab = tabWidget->widget(index);
  tabWidget->removeTab(index);
  tab->deleteLater();
}
