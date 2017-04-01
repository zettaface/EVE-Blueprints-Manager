#include "blueprintswidget.h"

#include <QDir>
#include <QDirIterator>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>

#include <fstream>

#include "blueprintpage.h"

BlueprintsWidget::BlueprintsWidget(QWidget* parent) : QWidget(parent)
{
  QAbstractItemModel* model = makeBlueprintsListModel();
  QWidget* bpConfigCont = makeBpConfigWidget();
  QPushButton* minimizeBpConfigBtn = makeMinimizeBpConfigButton(bpConfigCont);

  blueprintsView = setupBlueprintsView(model);

  QWidget* bpCont = new QWidget(this);
  QVBoxLayout* bpLayout = new QVBoxLayout(bpCont);
  bpLayout->addWidget(bpConfigCont);
  bpLayout->addWidget(minimizeBpConfigBtn);
  bpLayout->addWidget(blueprintsView);

  bpLayout->setStretch(0, 0);
  bpLayout->setStretch(1, 0);
  bpLayout->setStretch(2, 5);

  tabWidget = new TabWidget(this);
  tabWidget->addTab(bpCont, "Blueprints");
  tabWidget->setTabsClosable(true);
  tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);

  connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabClose(int)));

  QVBoxLayout* l = new QVBoxLayout(this);
  l->setSpacing(0);
  l->setMargin(1);
  l->addWidget(tabWidget);

  connect(bpFilterProxy_, SIGNAL(invalidated()), this, SIGNAL(modelUpdated()), Qt::QueuedConnection);
  connect(blueprintsModel, SIGNAL(modelReset()), this, SIGNAL(modelUpdated()));
  connect(blueprintsView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onBpTabRequest(QModelIndex)));
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

  bpFilterProxy_->setFilter(BpFilterFactory::instance().createFilter(BpFilterFactory::Logical));

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


QWidget* BlueprintsWidget::makeBpConfigWidget()
{
  QGroupBox* optionsGroupBox = makeOptionsBox();
  QGroupBox* sortGroupBox = makeSortBox();
  QGroupBox* groupingBox = makeBlueprintsGroupingBox();

  QVBoxLayout* leftMiddleVUnionL = makeOptionsBoxesLayout(sortGroupBox, groupingBox, optionsGroupBox);
  QGroupBox* filtersGroupBox = makeFiltersBox();

  QHBoxLayout* bpConfigL = new QHBoxLayout;
  bpConfigL->setMargin(0);
  bpConfigL->addLayout(leftMiddleVUnionL);
  bpConfigL->addWidget(filtersGroupBox);

  QWidget* bpConfigCont = new QWidget(this);
  bpConfigCont->setLayout(bpConfigL);
  bpConfigCont->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  return bpConfigCont;
}

QGroupBox* BlueprintsWidget::makeOptionsBox()
{
  QCheckBox* flatListCheckBox = new QCheckBox("Flat", this);
  QCheckBox* colorizedCheckBox = new QCheckBox("No color", this);
  QCheckBox* showMissingBpBox = new QCheckBox("Show missing blueprints", this);

  QGroupBox* optionsGroupBox = new QGroupBox("Options", this);
  QVBoxLayout* optionsL = new QVBoxLayout(optionsGroupBox);
  optionsL->addWidget(flatListCheckBox);
  optionsL->addWidget(colorizedCheckBox);
  optionsL->addWidget(showMissingBpBox);
  optionsL->setSpacing(2);

  connect(flatListCheckBox, SIGNAL(toggled(bool)), blueprintsModel, SLOT(setFlat(bool)));
  connect(flatListCheckBox, SIGNAL(toggled(bool)), bpEmptyGroupsProxy_, SLOT(setDisabled(bool)));
  connect(flatListCheckBox, SIGNAL(toggled(bool)), blueprintsModel, SLOT(loadFromMemory()));
  connect(colorizedCheckBox, SIGNAL(toggled(bool)), this, SLOT(setBpModelColorized(bool)));
  connect(showMissingBpBox, SIGNAL(toggled(bool)), blueprintsModel, SLOT(setShowAll(bool)));
  connect(showMissingBpBox, SIGNAL(toggled(bool)), blueprintsModel, SLOT(loadFromMemory()));

  return optionsGroupBox;
}

QGroupBox* BlueprintsWidget::makeSortBox()
{
  QComboBox* sortComboBox = new QComboBox(this);
  sortComboBox->addItem("Name", BlueprintsSortProxy::Name);
  sortComboBox->addItem("Runs", BlueprintsSortProxy::Runs);
  sortComboBox->addItem("Quantity", BlueprintsSortProxy::Quantity);
  sortComboBox->addItem("Location", BlueprintsSortProxy::Location);
  sortComboBox->addItem("ME", BlueprintsSortProxy::ME);
  sortComboBox->addItem("TE", BlueprintsSortProxy::TE);
  sortComboBox->addItem("Color(experimental)", BlueprintsSortProxy::Color);

  connect(sortComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sortBlueprints(int)));

  QComboBox* sortOrderComboBox = new QComboBox(this);
  sortOrderComboBox->addItem("Ascending", QVariant::fromValue(Qt::AscendingOrder));
  sortOrderComboBox->addItem("Descending", QVariant::fromValue(Qt::DescendingOrder));

  auto ltype =  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);

  connect(sortOrderComboBox, ltype, [this, sortComboBox, sortOrderComboBox](int) {
    bpSortProxy_->setSortOrder(sortOrderComboBox->currentData().value<Qt::SortOrder>());
    sortBlueprints(sortComboBox->currentData().toInt());
  });

  QGroupBox* sortGroupBox = new QGroupBox("Sort by...", this);
  QVBoxLayout* sortBoxL = new QVBoxLayout(sortGroupBox);
  sortBoxL->addWidget(sortComboBox);
  sortBoxL->addWidget(sortOrderComboBox);

  return sortGroupBox;
}

QGroupBox* BlueprintsWidget::makeBlueprintsGroupingBox()
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

QVBoxLayout* BlueprintsWidget::makeOptionsBoxesLayout(QGroupBox* sortGroupBox, QGroupBox* groupingBox, QGroupBox* optionsGroupBox)
{
  QVBoxLayout* sortOptionsLayout = new QVBoxLayout;
  sortOptionsLayout->setMargin(0);
  sortOptionsLayout->setSpacing(1);
  sortOptionsLayout->addWidget(sortGroupBox);
  sortOptionsLayout->addWidget(optionsGroupBox);
  sortOptionsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum));
  sortOptionsLayout->setStretch(2, 5);

  QHBoxLayout* hUnion = new QHBoxLayout;
  hUnion->addLayout(sortOptionsLayout);
  hUnion->addWidget(groupingBox);

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addLayout(hUnion);
  layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

  return layout;
}

QPushButton* BlueprintsWidget::makeMinimizeBpConfigButton(QWidget* bpConfigCont)
{
  QPushButton* minimizeBpConfigBtn = new QPushButton(QChar(0x02C4), this);
  minimizeBpConfigBtn->setCheckable(true);
  minimizeBpConfigBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
  connect(minimizeBpConfigBtn, &QPushButton::toggled, [bpConfigCont, minimizeBpConfigBtn](bool checked) {
    bpConfigCont->setVisible(!checked);
    if(checked)
      minimizeBpConfigBtn->setText(QChar(0x02C5));
    else
      minimizeBpConfigBtn->setText(QChar(0x02C4));
  });

  return minimizeBpConfigBtn;
}

QGroupBox* BlueprintsWidget::makeFiltersBox()
{
  using Type = BpFilterFactory::Type;
  QSet<int> filterTypes_t = BpFilterFactory::instance().availableFilters().subtract({Type::IsHeader});

  auto filter = qSharedPointerCast<BpComplexFilter>(bpFilterProxy_->filter());
  filterBox = new BlueprintsFilterBoxWidget(filterTypes_t, filter, this);

  QPushButton* loadFilterBtn = new QPushButton("Load", this);
  loadFilterBtn->setMenu(makeLoadFiltersMenu());

  QPushButton* saveFiltersBtn = new QPushButton("Save", this);
  connect(saveFiltersBtn, SIGNAL(clicked(bool)), this, SLOT(saveFilter()));
  connect(saveFiltersBtn, &QPushButton::clicked, [this, loadFilterBtn](bool) {
    QMenu* oldMenu = loadFilterBtn->menu();
    loadFilterBtn->setMenu(makeLoadFiltersMenu());
    oldMenu->deleteLater();
  });

  QVBoxLayout* filtersLayout = new QVBoxLayout;
  filtersLayout->setAlignment(Qt::AlignTop);
  filtersLayout->setSpacing(3);

  QGroupBox* filtersGroupBox = new QGroupBox("Filters", this);
  filtersGroupBox->setLayout(filtersLayout);

  QHBoxLayout* filterButtonsL = new QHBoxLayout;
  filterButtonsL->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
  filterButtonsL->addWidget(saveFiltersBtn);
  filterButtonsL->addWidget(loadFilterBtn);
  filterButtonsL->setSpacing(0);
  filterButtonsL->setMargin(1);

  filtersLayout->addWidget(filterBox);
  filtersLayout->addLayout(filterButtonsL);

  return filtersGroupBox;
}


BpView* BlueprintsWidget::setupBlueprintsView(QAbstractItemModel* model)
{
  BpView* view = new BpView(this);
  view->setModel(model);
  view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  view->setSelectionMode(QAbstractItemView::ContiguousSelection);
  view->setSelectionBehavior(QAbstractItemView::SelectItems);
  view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  view->setUniformRowHeights(true);
  view->header()->setSectionResizeMode(Blueprint::Activity, QHeaderView::Fixed);
  view->setColumnWidth(Blueprint::Activity, 24);

  view->resizeColumnToContents(Blueprint::Name);

  return view;
}

BlueprintsWidget::~BlueprintsWidget()
{
  QSettings settings;
  QVariantList filteredColumns;

  for (int column : bpColumnFilter_->filteredColumns())
    filteredColumns << column;

  settings.setValue("mainWindow/filteredColumns", filteredColumns);
}

void BlueprintsWidget::clearCache()
{
  bpColorizer->clearCache();
  bpDecorationsProxy->clearCache();
  bpAggregationProxy_->clearCache();
}

QString BlueprintsWidget::aggregationInfo() const
{
  QString aggregationInfo("%2(%3 unique) blueprints, %1 runs total");
  aggregationInfo = aggregationInfo.arg(bpAggregationProxy_->totalRuns()).arg(bpAggregationProxy_->totatQuantity()).arg(bpAggregationProxy_->rowCount());

  return aggregationInfo;
}


QMenu* BlueprintsWidget::makeLoadFiltersMenu() const
{
  QMenu* root = new QMenu();

  QAction* loadFromFileA = root->addAction("Load from file...", []() { qDebug() << "loading from file"; });
  connect(loadFromFileA, &QAction::triggered, [this](bool) {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open filter", "Filters", "YAML(*.yaml)");

    if (fileName.isEmpty())
      return;

    loadFilter(fileName);
  });

  QDir rootDir = QDir::current();
  if (!rootDir.cd("Filters"))
    return root;

  root->addSeparator();
  makeLoadFiltersMenu(rootDir, root);

  return root;
}

void BlueprintsWidget::makeLoadFiltersMenu(QDir dir, QMenu* root) const
{
  QDirIterator subCatIt(dir.absolutePath(), QDir::AllDirs | QDir::NoDotAndDotDot);

  while (subCatIt.hasNext()) {
    QString fullPath = subCatIt.next();
    QString name = subCatIt.fileName();
    QMenu* subMenu = new QMenu(name, root);

    makeLoadFiltersMenu(QDir(fullPath), subMenu);

    if (subMenu->actions().size() > 0)
      root->addMenu(subMenu);
    else
      subMenu->deleteLater();
  }

  QDirIterator filtersIt(dir.absolutePath(), QStringList() << "*.yaml", QDir::Files | QDir::NoDotAndDotDot );
  while (filtersIt.hasNext()) {
    QString fullPath = filtersIt.next();
    QString name = filtersIt.fileInfo().completeBaseName();

    QAction* action = root->addAction(name);
    connect(action, &QAction::triggered, [this, fullPath](bool) {
      loadFilter(fullPath);
    });
  }
}

void BlueprintsWidget::loadFilter(const QString& filename)
{
  if (filename.isEmpty())
    return;

  BpFilterFactory factory;
  QSharedPointer<BpFilter> ptr = factory.loadFilters(YAML::LoadFile(filename.toStdString()));

  if (ptr.isNull())
    qWarning() << "NULL FILTER POINTER";
  else {
    filterBox->setFilter(qSharedPointerCast<BpComplexFilter>(ptr));
    bpFilterProxy_->setFilter(ptr);
  }
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

void BlueprintsWidget::sortBlueprints(int type)
{
  using Columns = Blueprint::Columns;
  using Type = BlueprintsSortProxy::Type;

  bpSortProxy_->setType(type);

  switch(type) {
    case Type::Name:
      bpSortProxy_->sort(Columns::Name);
      break;
    case Type::Location:
      bpSortProxy_->sort(Columns::LocationIDc);
      break;
    case Type::Runs:
      bpSortProxy_->sort(Columns::Runs);
      break;
    case Type::Quantity:
      bpSortProxy_->sort(Columns::Quantity);
      break;
    case Type::ME:
      bpSortProxy_->sort(Columns::MEc);
      break;
    case Type::TE:
      bpSortProxy_->sort(Columns::TEc);
      break;
    case Type::Color:
      bpSortProxy_->sort(Columns::Name);
      break;
    default:
      qDebug() << "Unkonwn sort column. Got " << type;
      break;
  }
}

void BlueprintsWidget::saveFilter()
{
  BpFilterFactory factory;
  QString fileName = QFileDialog::getSaveFileName(this, "Save filter", "Filters", "YAML(*.yaml)");

  if (fileName.isEmpty())
    return;

  std::ofstream yOut(QString("%1.yaml").arg(fileName.split(".").at(0)).toStdString());
  yOut << factory.saveFiltersAsYaml(filterBox->filter());
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
