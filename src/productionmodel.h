#ifndef PRODUCTIONMODEL_H
#define PRODUCTIONMODEL_H

#include <QAbstractItemModel>

#include "productiontype.h"

class ProductionModel : public QAbstractItemModel
{
    Q_OBJECT
  public:
    explicit ProductionModel(QObject* parent = nullptr) : QAbstractItemModel(parent) {}
    explicit ProductionModel(const Blueprint& bp, QObject* parent = 0);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool hasChildren(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;
    bool canFetchMore(const QModelIndex& parent) const override;

    ProductionType* product() const;

    enum Columns
    {
      NameColumn = 0,
      MEColumn,
      TEColumn,
      QuantityColumn,

      BuildBuyColumn,
      MarketPriceType,
      MarketPriceColumn,
      BuildPriceColumn,
      TotalColumn,

      ColumnCount
    };

  signals:
    void priceChanged(double);

  private:
    void initComponent(ProductionType*) const;
    ProductionType* root_;

  private slots:
    void refreshPrice(const QModelIndex&, const QModelIndex&, QVector<int>);
};

#endif // PRODUCTIONMODEL_H
