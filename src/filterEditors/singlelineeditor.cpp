#include "singlelineeditor.h"

#include <QHBoxLayout>

SingleLineEditor::SingleLineEditor(QWidget* parent) :
  BpFilterEditor(parent)
{
  editor = new QLineEdit(this);

  connect(editor, SIGNAL(textChanged(QString)), this, SLOT(updateValue(QString)));

  QHBoxLayout* l = new QHBoxLayout(this);
  l->addWidget(editor);
  l->setMargin(0);
  l->setSpacing(0);
}

SingleLineEditor::SingleLineEditor(QSharedPointer<BpFilter> filter, QWidget* parent):
  SingleLineEditor(parent)
{
  setFilter(filter);
}

void SingleLineEditor::setDefaultValue()
{
  editor->setText(defaultValue_);
  updateValue(defaultValue_);
}

void SingleLineEditor::updateValue(QString str)
{
  if (filter_.isNull()) {
    qDebug() << "No filter installed in editor";
    return;
  }

  filter_->setValue(str);
}

void SingleLineEditor::setEditorValue(const QVariant& val)
{
  editor->setText(val.toString());
}
