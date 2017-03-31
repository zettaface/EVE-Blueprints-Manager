#include "productionmodelitemdelegate.h"

#include <QComboBox>

#include "productionmodel.h"

QWidget* ProductionModelItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  switch(index.column()) {
    case ProductionModel::BuildBuyColumn:
      return createBuildBuyEditor(parent, index);
    case ProductionModel::MarketPriceType:
      return createMarketPriceTypeEditor(parent, index);
    default:
      return QStyledItemDelegate::createEditor(parent, option, index);
  }
}

void ProductionModelItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  switch(index.column()) {
    case ProductionModel::BuildBuyColumn:
      return setBuildBuyEditorData(editor, index);
    case ProductionModel::MarketPriceType:
      return setMarketPriceTypeEditorData(editor, index);
    default:
      return QStyledItemDelegate::setEditorData(editor, index);
  }
}

void ProductionModelItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  switch(index.column()) {
    case ProductionModel::BuildBuyColumn:
      return setBuildBuy(editor, model, index);
    case ProductionModel::MarketPriceType:
      return setMarketPriceType(editor, model, index);
    default:
      return QStyledItemDelegate::setModelData(editor, model, index);
  }
}

void ProductionModelItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

QWidget* ProductionModelItemDelegate::createBuildBuyEditor(QWidget* parent, const QModelIndex& index) const
{
  QComboBox* box = new QComboBox(parent);

  box->insertItem(0, "Buy", false);
  box->insertItem(1, "Build", true);

  connect(box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this, box](int) mutable {
    //FIXME: Temporary workaround: commit in const method
    auto ncThis = const_cast<ProductionModelItemDelegate*>(this);
    ncThis->commitData(box);
    ncThis->closeEditor(box);
  });

  return box;

}

void ProductionModelItemDelegate::setBuildBuyEditorData(QWidget* editor, const QModelIndex& index) const
{
  QComboBox* box = static_cast<QComboBox*>(editor);
  box->setCurrentIndex(index.data(Qt::UserRole).toBool());
}

void ProductionModelItemDelegate::setBuildBuy(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  QComboBox* box = static_cast<QComboBox*>(editor);
  model->setData(index, box->currentData());
}

QWidget* ProductionModelItemDelegate::createMarketPriceTypeEditor(QWidget* parent, const QModelIndex& index) const
{
  QComboBox* editor = new QComboBox(parent);
  using Type = PriceInfo::PriceType;

  QMap<int, QString> vals {
    {Type::SellMin, "Min Sell"},
    {Type::SellMax, "Max Sell"},
    {Type::SellAvg, "Avg Sell"},
    {Type::SellMedian, "Median Sell"},
    {Type::BuyMin, "Min Buy"},
    {Type::BuyMax, "Max Buy"},
    {Type::BuyAvg, "Avg Buy"},
    {Type::BuyMedian, "Median Buy"},
    {Type::User, "User price"}
  };

  auto keys = vals.keys();

  for (int key : keys) {
    marketTypeToEditorID_[key] = editor->count();
    editor->addItem(vals[key], key);
  }

  return editor;
}

void ProductionModelItemDelegate::setMarketPriceTypeEditorData(QWidget* editor, const QModelIndex& index) const
{
  QComboBox* box = static_cast<QComboBox*>(editor);
  int type = index.data(Qt::UserRole).toInt();

  box->setCurrentIndex(marketTypeToEditorID_[type]);

}

void ProductionModelItemDelegate::setMarketPriceType(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  QComboBox* box = static_cast<QComboBox*>(editor);

  model->setData(index, box->currentData());
}
