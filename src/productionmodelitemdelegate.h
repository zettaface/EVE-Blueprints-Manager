#ifndef PRODUCTIONMODELITEMDELEGATE_H
#define PRODUCTIONMODELITEMDELEGATE_H

#include <QStyledItemDelegate>

class ProductionModelItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
  public:
    ProductionModelItemDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
  private:
    QWidget* createBuildBuyEditor(QWidget* parent,  const QModelIndex& index) const;
    void setBuildBuyEditorData(QWidget* editor, const QModelIndex& index) const;
    void setBuildBuy(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

    QWidget* createMarketPriceTypeEditor(QWidget* parent,  const QModelIndex& index) const;
    void setMarketPriceTypeEditorData(QWidget* editor, const QModelIndex& index) const;
    void setMarketPriceType(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

    mutable QMap <int, int> marketTypeToEditorID_;
};

#endif // PRODUCTIONMODELITEMDELEGATE_H
