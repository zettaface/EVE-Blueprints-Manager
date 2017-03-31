#include "bpfiltereditor.h"


BpFilterEditor::BpFilterEditor(QWidget* parent) :
  QWidget(parent)
{
}

BpFilterEditor::BpFilterEditor(QSharedPointer<BpFilter> filter, QWidget* parent) :
  filter_(filter), QWidget(parent)
{

}

QSharedPointer<BpFilter> BpFilterEditor::filter() const
{
  return filter_;
}

void BpFilterEditor::setFilter(const QSharedPointer<BpFilter>& filter)
{
  filter_ = filter;
  setEditorValue(filter_->value());
  initialized_ = true;
}

void BpFilterEditor::showEvent(QShowEvent* e)
{
  QWidget::showEvent(e);
}

void BpFilterEditor::initialize()
{
  setDefaultValue();
  initialized_ = true;
}
