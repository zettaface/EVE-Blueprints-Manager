#include "categorygroupfiltereditor.h"

#include <QHBoxLayout>
#include <QComboBox>

#include "../eve/staticdata.h"

CategoryGroupFilterEditor::CategoryGroupFilterEditor(QWidget* parent) :
  BpFilterEditor(parent)
{
  categoryBox = new QComboBox(this);
  categoryBox->setModel(eve::SDE.bpCategoriesModel());

  groupBox = new QComboBox(this);

  QAbstractItemModel* groupBoxModel = eve::SDE.bpGroupModel();
  groupBoxFilter = new CategoryFilterProxyModel(groupBoxModel);
  groupBoxFilter->setFilterRole(eve::BPGroupsModel::DataRole::CategoryID);
  groupBoxFilter->setSourceModel(groupBoxModel);
  groupBox->setModel(groupBoxFilter);
  categoryBox->setMinimumWidth(50);
  groupBox->setMinimumWidth(50);

  connect(categoryBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCategory(int)));
  connect(groupBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGroup(int)));

  QHBoxLayout* l = new QHBoxLayout(this);
  l->addWidget(categoryBox);
  l->addWidget(groupBox);
  l->setMargin(0);
  l->setSpacing(3);

  l->setStretch(0, 1);
  l->setStretch(1, 1);

}

CategoryGroupFilterEditor::CategoryGroupFilterEditor(QSharedPointer<BpFilter> filter, QWidget* parent) :
  CategoryGroupFilterEditor(parent)
{
  filter_ = filter;
  setEditorValue(filter_->value());
}

void CategoryGroupFilterEditor::setDefaultValue()
{
  categoryBox->setCurrentIndex(0);
  groupBox->setCurrentIndex(0);

  updateProxy(-1, -1);
}

void CategoryGroupFilterEditor::updateCategory(int)
{
  int cid = categoryBox->currentData().toInt();

  groupBoxFilter->setCategoryID(cid);
  updateProxy(cid, -1);
}

void CategoryGroupFilterEditor::updateGroup(int)
{
  int cid = categoryBox->currentData().toInt();
  int gid = groupBox->currentData(eve::BPGroupsModel::DataRole::GroupID).toInt();

  qDebug() << cid << gid;

  updateProxy(cid, gid);
}

void CategoryGroupFilterEditor::updateProxy(int cid, int gid)
{
  if (filter_.isNull()) {
    qDebug() << "No filter installed in editor";
    return;
  }

  filter_->setValue(QVariant::fromValue(QPair<int, int>(cid, gid)));
}

void CategoryGroupFilterEditor::setEditorValue(const QVariant& val)
{
  QPair<int, int> p = val.value<QPair<int, int>>();
  int row = findCategoryRow(p.first);
  int grow = findGroupRow(p.second);

  categoryBox->setCurrentIndex(row);
  groupBoxFilter->setCategoryID(p.first);
  groupBox->setCurrentIndex(grow);
}

int CategoryGroupFilterEditor::findCategoryRow(int ID)
{
  QAbstractItemModel* model = categoryBox->model();
  for (int i = 0; i < model->rowCount(); i++) {
    if (ID == model->data(model->index(i, 0), Qt::UserRole).toInt())
      return i;
  }

  return -1;
}

int CategoryGroupFilterEditor::findGroupRow(int ID)
{
  QAbstractItemModel* model = groupBox->model();
  for (int i = 0; i < model->rowCount(); i++) {
    if (ID == model->data(model->index(i, 0), Qt::UserRole + 1).toInt())
      return i;
  }

  return -1;
}
