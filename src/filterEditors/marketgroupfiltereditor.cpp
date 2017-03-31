#include "marketgroupfiltereditor.h"

#include <QHBoxLayout>

#include "../eve/staticdata.h"

MarketGroupFilterEditor::MarketGroupFilterEditor(QWidget* parent) :
  BpFilterEditor(parent)
{
  marketIDFilterBox = new HierarchicalComboBox(this);
  marketIDFilterBox->setModel(eve::SDE.marketGroupsModel());
  marketIDFilterBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

  connect(marketIDFilterBox, SIGNAL(activated(int)), this, SLOT(updateMarketGroup(int)));

  QHBoxLayout* l = new QHBoxLayout(this);
  l->addWidget(marketIDFilterBox);
  l->setMargin(0);
  l->setSpacing(3);
}

MarketGroupFilterEditor::MarketGroupFilterEditor(QSharedPointer<BpFilter> filter, QWidget* parent):
  MarketGroupFilterEditor(parent)
{
  setFilter(filter);
}

void MarketGroupFilterEditor::setDefaultValue()
{
  marketIDFilterBox->setCurrentIndex(0);
  updateMarketGroup(0);
}

void MarketGroupFilterEditor::updateMarketGroup(int)
{
  if (filter_.isNull()) {
    qDebug() << "No filter installed in editor";
    return;
  }

  filter_->setValue(marketIDFilterBox->currentData(Qt::UserRole + 1));
}

void MarketGroupFilterEditor::setEditorValue(const QVariant& val)
{
  MarketGroupsModel* model = eve::SDE.marketGroupsModel();
  QModelIndex index = model->index(val.toInt(), 0);
  marketIDFilterBox->selectIndex(index);
}
