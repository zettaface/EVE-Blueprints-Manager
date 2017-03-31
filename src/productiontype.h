#ifndef PRODUCTIONTYPE_H
#define PRODUCTIONTYPE_H

#include <QObject>
#include <QVector>
#include <QString>

#include "blueprint.h"
#include "priceinfo.h"

class ProductionType : public QObject
{
    Q_OBJECT
  public:
    ProductionType(const Blueprint&, ProductionType* parent = 0);
    ProductionType(long typeID, long bpID, int quantity, ProductionType* parent = 0);

    QString name() const;
    int baseQuantity() const;
    int modifiedQuantity() const;

    ProductionType* parent() const;
    void addComponent(ProductionType*);
    ProductionType* component(int row) const;
    int componentsCount() const;

    double pricePerItem() const;
    double price() const;

    double buildPrice();
    double marketPrice();

    bool isFinal() const;

    int row() const;

    long blueprintID() const;
    long productID() const;
    int ME() const;
    int TE() const;

    void setME(int ME);
    void setTE(int TE);

    PriceInfo* priceInfo() const;
    void setPriceInfo(PriceInfo* priceInfo);

    bool build() const;
    void setBuild(bool build);

    QVariant data(int column) const;
    QVariant displayData(int column) const;

    static QString formatPrice(double);

    enum Columns {
      MarketPrice,
      BuildPrice,
      TotalPrice
    };

    int priceType() const;
    void setPriceType(int priceType);

  public slots:
    void refreshPrice();

  private:
    ProductionType* parent_ { nullptr } ;
    QVector<ProductionType*> components_;

    QString name_;
    int baseQuantity_ { 1};
    long blueprintID_ {-1};
    long productID_   {-1};
    int ME_ {0};
    int TE_ {0};

    int priceType_;

    double price_ {-1.0};
    PriceInfo* priceInfo_ {nullptr};

    bool build_ {false};
  signals:
    void priceUpdated(double);
  private:

    friend class ProductionModel;
};

#endif // PRODUCTIONTYPE_H
