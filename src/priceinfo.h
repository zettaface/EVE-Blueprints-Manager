#ifndef PRICEINFO_H
#define PRICEINFO_H

#include <QMetaType>
#include <QObject>
#include <QString>
#include <QVector>

class PriceInfo : public QObject
{
    Q_OBJECT
  public:
    struct Order;

    PriceInfo() = default;
    PriceInfo(int typeID);
    void initFrom(const PriceInfo&);

    double sellAvg() const;
    double sellMin() const;
    double sellMax() const;
    double sellStdDev() const;
    double sellMedian() const;

    double buyAvg() const;
    double buyMin() const;
    double buyMax() const;
    double buyStdDev() const;
    double buyMedian() const;

    double price();
    double price(int type);

    long sellVolume() const;
    long buyVolume() const;

    void setSellAvg(double sellAvg);
    void setSellMin(double sellMin);
    void setSellMax(double sellMax);
    void setSellStdDev(double sellStdDev);
    void setSellMedian(double sellMedian);
    void setBuyAvg(double buyAvg);
    void setBuyMin(double buyMin);
    void setBuyMax(double buyMax);
    void setBuyStdDev(double buyStdDev);
    void setBuyMedian(double buyMedian);

    void setSellVolume(long sellVolume);
    void setBuyVolume(long buyVolume);

    bool initialized() const;
    void setInitialized(bool initialized);

    int typeID() const;

    void addSellOrder(double price, long quantity);
    void addSellOrder(Order order);
    void addBuyOrder(double price, long quantity);
    void addBuyOrder(Order order);
    void updatePrices();

    static QString formatPrice(double, int prec = 2, const QString& currency = "ISK");

    enum PriceType
    {
      SellMin = 0,
      SellMax,
      SellAvg,
      SellMedian,
      BuyMin,
      BuyMax,
      BuyAvg,
      BuyMedian,
      SellStdDev,
      BuyStdDev,
      User
    };

    double userPrice() const;
    void setUserPrice(double userPrice);

    int getPriceType() const;
    void setPriceType(int value);

  private:
    QVector<Order> sellOrders_;
    QVector<Order> buyOrders_;

    double sellAvg_    {-1};
    double sellMin_    {-1};
    double sellMax_    {-1};
    double sellStdDev_ {-1};
    double sellMedian_ {-1};
    long sellVolume_   {-1};

    double buyAvg_     {-1};
    double buyMin_     {-1};
    double buyMax_     {-1};
    double buyStdDev_  {-1};
    double buyMedian_  {-1};
    long buyVolume_    {-1};

    double userPrice_ {0.};

    int typeID_        {0};
    int priceType      {SellMin};

    bool initialized_  {false};
  signals:
    void updated();
};

struct PriceInfo::Order
{
  long   quantity;
  double price;
};

#endif // PRICEINFO_H
