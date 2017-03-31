#include "blueprintsfilterboxwidget.h"

#include <QDebug>
#include <QPainter>

#include <memory>

#include "bpfilterfactory.h"

BlueprintsFilterBoxWidget::BlueprintsFilterBoxWidget(QSharedPointer<BpComplexFilter> filter, QWidget* parent) :
  BlueprintsFilterBoxWidget(BpFilterFactory::instance().availableFilters(), filter, parent)
{
}

BlueprintsFilterBoxWidget::BlueprintsFilterBoxWidget(const QSet<int>& filterTypes, QSharedPointer<BpComplexFilter> filter, QWidget* parent) :
  BlueprintsFilterBoxWidget(filterTypes.toList().toVector(), filter, parent)
{

}

BlueprintsFilterBoxWidget::BlueprintsFilterBoxWidget(const QVector<int>& filterTypes, QSharedPointer<BpComplexFilter> filter, QWidget* parent) :
  filterTypes_(filterTypes), QWidget(parent), filter_(filter)
{
  std::sort(filterTypes_.begin(), filterTypes_.end());
  filtersLayout = new QVBoxLayout(this);

  if (!filter_.isNull())
    setFilter(filter_);

  filtersLayout->setMargin(0);
  filtersLayout->setSpacing(1);

  setAutoFillBackground(true);
}

BlueprintsFilterBoxWidget::BlueprintsFilterBoxWidget(QWidget* parent) :
  BlueprintsFilterBoxWidget(qSharedPointerCast<BpComplexFilter>(BpFilterFactory::instance().createFilter(BpFilterFactory::Logical)), parent)
{
}

BlueprintsFilterBoxWidget::BlueprintsFilterBoxWidget(const QSet<int>& filterTypes, QWidget* parent)
{

}

void BlueprintsFilterBoxWidget::addFilter()
{
  BlueprintsFilterWidget* bfw = new BlueprintsFilterWidget(filterTypes_, QSharedPointer<BpFilter>(), this);
  addFilterWidget(bfw);
}

void BlueprintsFilterBoxWidget::addFilter(QSharedPointer<BpFilter> filter)
{
  BlueprintsFilterWidget* bfw = new BlueprintsFilterWidget(filterTypes_, filter, this);
  addFilterWidget(bfw);
}

void BlueprintsFilterBoxWidget::addFilterWidget(BlueprintsFilterWidget* bfw)
{

  bfw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

  QPushButton* closeButton = new QPushButton(this);
  closeButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
  closeButton->setFixedWidth(8);
  closeButton->setStyleSheet("background-color: #ff7373; padding: 0px;");
  closeButton->setToolTip("Remove filter");
  if (bfw->currentFilter().isNull())
    closeButton->setVisible(false);

  QHBoxLayout* rowLayout = new QHBoxLayout;
  rowLayout->addWidget(bfw, 1);
  rowLayout->addWidget(closeButton, 0);

  QWidget* row = new QWidget(this);
  closeButton->setParent(row);
  bfw->setParent(row);

  row->setLayout(rowLayout);
  rowLayout->setMargin(1);
  rowLayout->setSpacing(1);

  filtersLayout->addWidget(row, 0, Qt::AlignTop);

  connect(bfw, &BlueprintsFilterWidget::filterTypeChanged, this, &BlueprintsFilterBoxWidget::onFilterTypeChange);
  connect(bfw, &BlueprintsFilterWidget::filterTypeChanged, [closeButton](QSharedPointer<BpFilter>, QSharedPointer<BpFilter>) {
    closeButton->setVisible(true);
  });

  connect(closeButton, &QPushButton::clicked, [this, row, bfw]() {
    removeFilter(row, bfw->currentFilter());
  });
}

void BlueprintsFilterBoxWidget::installFilter(QSharedPointer<BpFilter> filter)
{
  filter_->insertFilter(filter);
}

void BlueprintsFilterBoxWidget::setFilter(const QSharedPointer<BpComplexFilter>& filter)
{
  if (filter.isNull()) {
    qDebug() << "Null filter installed";
    return;
  }

  QLayoutItem* item;
  while ((item = filtersLayout->takeAt(0)) != nullptr) {
    delete item->widget();
    delete item;
  }

  auto& childs = filter->childs();

  for (auto child : childs)
    addFilter(child);

  addFilter();
  filter_ = filter;
}

QSharedPointer<BpComplexFilter> BlueprintsFilterBoxWidget::filter() const
{
  return filter_;
}

void BlueprintsFilterBoxWidget::removeFilter(QWidget* w, QSharedPointer<BpFilter> filter)
{
  filtersLayout->removeWidget(w);
  filter_->removeFilter(filter);

  w->deleteLater();
}

void BlueprintsFilterBoxWidget::onFilterTypeChange(QSharedPointer<BpFilter> oldFilter, QSharedPointer<BpFilter> newFilter)
{
  if (oldFilter == nullptr)
    filter_->insertFilter(newFilter);
  else
    filter_->swapFilter(oldFilter, newFilter);

  if (oldFilter == nullptr)
    addFilter();
}
