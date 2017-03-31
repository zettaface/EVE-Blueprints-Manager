#include "blueprintsfilterwidget.h"
#include "blueprintsfilterboxwidget.h"
#include "filterEditors/bpfiltereditor.h"

BlueprintsFilterWidget::BlueprintsFilterWidget(QWidget* parent) :
  QFrame(parent)
{
  setFrameStyle(QFrame::Panel | QFrame::Raised);
  setLineWidth(1);

  filterTypeBox = new QComboBox(this);
  filterTypeBox->addItem("None", 0);

  for (int i = 1; i < BpFilterFactory::TypeCount; i++)
    filterTypeBox->addItem(factory_.filterName(i), i);

  filterTypeBox->setItemData(0, QSize(0, 0), Qt::SizeHintRole);

  filters = new QStackedWidget(this);
  for (int i = 0; i < TypeCount; i++)
    filters->addWidget(new QWidget(this));

  filterTypeBox->setCurrentIndex(0);
  filterTypeBox->setMinimumHeight(20);

  QHBoxLayout* l = new QHBoxLayout(this);
  l->addWidget(filterTypeBox);
  l->addWidget(filters);
  l->setMargin(3);
  l->setSpacing(2);

  l->setAlignment(filterTypeBox, Qt::AlignTop);

  connect(filterTypeBox, SIGNAL(activated(int)), this, SLOT(onFilterTypeBoxActivated(int)));
}

BlueprintsFilterWidget::BlueprintsFilterWidget(QSharedPointer<BpFilter> filter, QWidget* parent) :
  BlueprintsFilterWidget(parent)
{
  setFilter(filter);
}

BlueprintsFilterWidget::BlueprintsFilterWidget(const QVector<int>& filterTypes, QSharedPointer<BpFilter> filter, QWidget* parent)
  : QFrame(parent), filterTypes_(filterTypes)
{
  setFrameStyle(QFrame::Panel | QFrame::Raised);
  setLineWidth(1);

  filterTypeBox = new QComboBox(this);
  filterTypeBox->addItem("None", 0);

  for (int f = 0; f < filterTypes.size(); f++) {
    filterTypeBox->insertItem(f + 1, factory_.filterName(filterTypes[f]), filterTypes[f]);
  }

  filterTypeBox->setItemData(0, QSize(0, 0), Qt::SizeHintRole);

  filters = new QStackedWidget(this);

  for (int i = 0; i < filterTypes.size(); i++)
    filters->addWidget(new QWidget(this));

  setFilter(filter);

  QHBoxLayout* l = new QHBoxLayout(this);
  l->addWidget(filterTypeBox, 0, Qt::AlignTop);
  l->addWidget(filters, 0, Qt::AlignTop);
  l->setMargin(3);
  l->setSpacing(2);

  connect(filterTypeBox, SIGNAL(activated(int)), this, SLOT(onFilterTypeBoxActivated(int)));
}

void BlueprintsFilterWidget::setType(int type)
{
  //Block scrolling to None type
  if (type == None) {
    filterTypeBox->setCurrentIndex(1);
    filters->setCurrentIndex(1);
    return;
  }

  if (!editors_.contains(type)) {
    QWidget* editor = getEditor(type);
    editors_.insert(type, editor);

    filters->addWidget(editor);
  }

  BpFilterEditor* editor = static_cast<BpFilterEditor*>(editors_.value(type));
  filters->setCurrentWidget(editor);
  QSharedPointer<BpFilter> newFilter = editor->filter();

  emit filterTypeChanged(currentFilter_, newFilter);

  currentFilter_ = newFilter;
}

void BlueprintsFilterWidget::setEditorDefaults()
{
  BpFilterEditor* editor = static_cast<BpFilterEditor*>(filters->currentWidget());
  editor->setDefaultValue();
}

void BlueprintsFilterWidget::onFilterTypeBoxActivated(int)
{
  setType(filterTypeBox->currentData().toInt());
}

QWidget* BlueprintsFilterWidget::getEditor(int ftype)
{
  return factory_.createFilterEditor(ftype, this);
}

QSharedPointer<BpFilter> BlueprintsFilterWidget::currentFilter() const
{
  return currentFilter_;
}

void BlueprintsFilterWidget::setFilter(QSharedPointer<BpFilter> filter)
{
  if (filter.isNull())
    return;

  int index = -1;
  QAbstractItemModel* filterTypeModel = filterTypeBox->model();
  for (int i = 0; i < filterTypeModel->rowCount(); i++) {
    if (filter->type() == filterTypeModel->index(i, 0).data(Qt::UserRole).toInt()) {
      index = i;
      break;
    }
  }

  setType(filter->type());
  filterTypeBox->setCurrentIndex(index);
  BpFilterEditor* editor = static_cast<BpFilterEditor*>(filters->currentWidget());
  editor->setFilter(filter);
  currentFilter_ = filter;
}
