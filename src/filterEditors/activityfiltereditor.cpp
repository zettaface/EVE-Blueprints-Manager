#include "activityfiltereditor.h"

#include <QVBoxLayout>

#include "../eve/staticdata.h"

ActivityFilterEditor::ActivityFilterEditor(QWidget* parent) :
  BpFilterEditor(parent)
{
  activityEditor = new QComboBox(this);
  activityEditor->setModel(eve::SDE.industryActivityModel());

  connect(activityEditor, SIGNAL(activated(int)), this, SLOT(onActivityTypeChanged(int)));

  QHBoxLayout* l = new QHBoxLayout(this);
  l->addWidget(activityEditor);
  l->setMargin(0);
}

ActivityFilterEditor::ActivityFilterEditor(QSharedPointer<BpFilter> filter, QWidget* parent):
  ActivityFilterEditor(parent)
{
  filter_ = filter;
  setEditorValue(filter_->value());
}

void ActivityFilterEditor::onActivityTypeChanged(int)
{
  if (filter_.isNull()) {
    qDebug() << "No filter installed in editor";
    return;
  }

  filter_->setValue(activityEditor->currentData());
}

void ActivityFilterEditor::setEditorValue(const QVariant& val)
{
  int index = -1;
  QAbstractItemModel* model = activityEditor->model();
  for (int i = 0; i < model->rowCount(); i++) {
    if (val.toInt() == model->index(i, 0).data(Qt::UserRole).toInt()) {
      index = i;
      break;
    }
  }

  activityEditor->setCurrentIndex(index);
}
