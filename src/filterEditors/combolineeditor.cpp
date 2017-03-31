#include "combolineeditor.h"

#include <QHBoxLayout>
#include <QPair>

#include "../filters/bpnumericfilter.h"

ComboLineEditor::ComboLineEditor(int func, QString val, QWidget* parent) :
  BpFilterEditor(parent),
  defaultFunc_(func),
  defaultValue_(val)
{
  valEdit = new QLineEdit(val, this);
  validator = new QIntValidator(valEdit);
  valEdit->setValidator(validator);

  funcBox = funcComboBox(this);
  funcBox->setCurrentIndex(func);

  connect(valEdit, SIGNAL(textChanged(QString)), this, SLOT(updateValue(QString)));
  connect(funcBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFunctor(int)));

  QHBoxLayout* l = new QHBoxLayout(this);
  l->addWidget(funcBox);
  l->addWidget(valEdit);
  l->setMargin(0);
  l->setSpacing(2);
}

void ComboLineEditor::setRange(int min, int max)
{
  validator->setRange(min, max);
}

void ComboLineEditor::setDefaultValue()
{
  funcBox->setCurrentIndex(defaultFunc_);
  valEdit->setText(defaultValue_);
  updateProxy(defaultValue_.toInt(), defaultFunc_);
}

void ComboLineEditor::updateValue(QString str)
{
  updateProxy(str.toInt(), funcBox->currentData().toInt());
}

void ComboLineEditor::updateFunctor(int)
{
  updateProxy(valEdit->text().toInt(), funcBox->currentData().toInt());
}

void ComboLineEditor::updateProxy(int val, int func)
{
  if (filter_.isNull()) {
    qDebug() << "No filter installed in editor";
    return;
  }

  filter_->setValue(QVariant::fromValue<QPair<int, int>>({val, func}));
}

QComboBox* ComboLineEditor::funcComboBox(QWidget* parent) const
{
  typedef BpNumericFilter::Type Func;
  QComboBox* box = new QComboBox(parent);

  box->insertItem(Func::LESS,          "<",  Func::LESS);
  box->insertItem(Func::LESS_EQUAL,    "<=", Func::LESS_EQUAL);
  box->insertItem(Func::GREATER,       ">",  Func::GREATER);
  box->insertItem(Func::GREATER_EQUAL, ">=", Func::GREATER_EQUAL);
  box->insertItem(Func::EQUAL,         "=",  Func::EQUAL);
  box->insertItem(Func::NOT_EQUAL,     "!=", Func::NOT_EQUAL);

  return box;
}

void ComboLineEditor::setEditorValue(const QVariant& val)
{
  QPair<int, int> p = val.value<QPair<int, int>>();
  valEdit->setText(QString::number(p.first));
  funcBox->setCurrentIndex(p.second);
}
