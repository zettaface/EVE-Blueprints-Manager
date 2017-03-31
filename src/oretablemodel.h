#ifndef ORETABLEMODEL_H
#define ORETABLEMODEL_H

#include <QAbstractTableModel>
#include <QPixmap>

#include "eve/staticdata.h"
#include "priceinfo.h"

struct ItemRefinery
{
  QString name;
  QVector<double> minerals;
  QPixmap icon;
  int skillID;
  double price;
  bool checked {false};
  PriceInfo* priceInfo;
};

class OreTableModel : public QAbstractTableModel
{
    Q_OBJECT

  public:
    explicit OreTableModel(QObject* parent = 0) : QAbstractTableModel(parent) {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVector<ItemRefinery> selectedItems() const;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    enum Columns
    {
      Name,
      Price,
      Tritanium,
      Pyerite,
      Mexallon,
      Isogen,
      Nocxium,
      Zydrine,
      Megacyte,
      ColumnsCount
    };

  public slots:
    void loadFromMemory();

  private:
    QVector<double> refineInfoToMinerals(const RefineInfo&) const;
    QVariant mineralDataColumn(const QModelIndex&) const;

    QVector<ItemRefinery> data_;
};

#endif // ORETABLEMODEL_H
