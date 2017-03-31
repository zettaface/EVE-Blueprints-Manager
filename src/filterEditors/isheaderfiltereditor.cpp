#include "isheaderfiltereditor.h"

#include <QHBoxLayout>

IsHeaderFilterEditor::IsHeaderFilterEditor(QWidget* parent) :
  BpFilterEditor(parent)
{
  editor = new QComboBox(this);
  editor->addItem("True", true);
  editor->addItem("False", false);

  connect(editor, SIGNAL(activated(int)), this, SLOT(onTypeChanged(int)));

  QHBoxLayout* l = new QHBoxLayout(this);
  l->addWidget(editor);
  l->setMargin(0);
}

void IsHeaderFilterEditor::onTypeChanged(int)
{
  if (filter_.isNull()) {
    qDebug() << "No filter installed in editor";
    return;
  }

  filter_->setValue(editor->currentData());
}

void IsHeaderFilterEditor::setEditorValue(const QVariant& val)
{
  int index = -1;

  QAbstractItemModel* model = editor->model();
  for (int i = 0; i < model->rowCount(); i++) {
    if (val.toBool() == model->index(i, 0).data(Qt::UserRole).toBool()) {
      index = i;
      break;
    }
  }

  editor->setCurrentIndex(index);
}
