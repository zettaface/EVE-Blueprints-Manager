#include "complexeditor.h"

#include <QVBoxLayout>

#include "../blueprintsfilterboxwidget.h"

class impComplexEditor
{
  public:
    impComplexEditor(QWidget* parent) {
      childFilters = new BlueprintsFilterBoxWidget(parent);
    }
    impComplexEditor(QSharedPointer<BpFilter> filter, QWidget* parent) {
      childFilters = new BlueprintsFilterBoxWidget(qSharedPointerCast<BpComplexFilter>(filter), parent);
    }

    BlueprintsFilterBoxWidget* childFilters;
};

ComplexEditor::ComplexEditor(QWidget* parent) :
  BpFilterEditor(parent)
{
  QVBoxLayout* l = new QVBoxLayout(this);
  l->setMargin(0);
  l->setSpacing(1);

  typeBox = new QComboBox(this);
  typeBox->addItem("AND", BpComplexFilter::LOGICAL_AND);
  typeBox->addItem("OR", BpComplexFilter::LOGICAL_OR);
  typeBox->addItem("NOT", BpComplexFilter::LOGICAL_NOT);
  typeBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  _imp = new impComplexEditor(filter_, this);
  _imp->childFilters->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  l->addWidget(typeBox, 0, Qt::AlignTop);
  l->addWidget(_imp->childFilters, 0, Qt::AlignTop);

  connect(typeBox, SIGNAL(activated(int)), this, SLOT(onFilterTypeChange()));

}

ComplexEditor::ComplexEditor(QSharedPointer<BpFilter> filter, QWidget* parent) :
  ComplexEditor(parent)
{
  setFilter(filter);
}

ComplexEditor::~ComplexEditor()
{
  delete _imp;
}

void ComplexEditor::onFilterTypeChange()
{
  if (filter_.isNull()) {
    qDebug() << "No filter installed in editor";
    return;
  }

  filter_->setValue(typeBox->currentData());
}

void ComplexEditor::setFilter(const QSharedPointer<BpFilter>& filter)
{
  if (filter->type() != BpFilterFactory::Logical) {
    qWarning() << "ComplexEditor::setFitler() - Wrong filter type";
    return;
  }

  filter_ = filter;
  _imp->childFilters->setFilter(qSharedPointerCast<BpComplexFilter>(filter));
  setEditorValue(filter->value());
}

void ComplexEditor::setEditorValue(const QVariant& val)
{
  int index = -1;
  QAbstractItemModel* model = typeBox->model();
  for (int i = 0; i < model->rowCount(); i++) {
    if (val.toInt() == model->index(i, 0).data(Qt::UserRole).toInt()) {
      index = i;
      break;
    }
  }

  typeBox->setCurrentIndex(index);
}
