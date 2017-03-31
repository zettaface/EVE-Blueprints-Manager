#include "priceinfo.h"

#include <QDebug>

#include <sstream>
#include <locale>

PriceInfo::PriceInfo(int typeID) :
  typeID_(typeID)
{

}

void PriceInfo::initFrom(const PriceInfo& p)
{
  sellAvg_    = p.sellAvg_;
  sellMin_    = p.sellMin_;
  sellMax_    = p.sellMax_;
  sellStdDev_ = p.sellStdDev_;
  sellMedian_ = p.sellMedian_;
  sellVolume_ = p.sellVolume_;

  buyAvg_     = p.buyAvg_;
  buyMin_     = p.buyMin_;
  buyMax_     = p.buyMax_;
  buyStdDev_  = p.buyStdDev_;
  buyMedian_  = p.buyMedian_;
  buyVolume_  = p.buyVolume_;

  typeID_     = p.typeID_;

  initialized_ = p.initialized_;

  sellOrders_ = p.sellOrders_;
  buyOrders_  = p.buyOrders_;
}

double PriceInfo::sellMin() const
{
  return sellMin_;
}

double PriceInfo::sellMax() const
{
  return sellMax_;
}

double PriceInfo::sellAvg() const
{
  return sellAvg_;
}

double PriceInfo::sellStdDev() const
{
  return sellStdDev_;
}

double PriceInfo::sellMedian() const
{
  return sellMedian_;
}

double PriceInfo::buyAvg() const
{
  return buyAvg_;
}

double PriceInfo::buyMin() const
{
  return buyMin_;
}

double PriceInfo::buyMax() const
{
  return buyMax_;
}

double PriceInfo::buyStdDev() const
{
  return buyStdDev_;
}

double PriceInfo::buyMedian() const
{
  return buyMedian_;
}

double PriceInfo::price()
{
  return price(priceType);
}

double PriceInfo::price(int type)
{
  switch (type) {
    case SellMin:
      return sellMin();
    case SellMax:
      return sellMax();
    case SellAvg:
      return sellAvg();
    case SellMedian:
      return sellMedian();
    case SellStdDev:
      return sellStdDev();
    case BuyMin:
      return buyMin();
    case BuyMax:
      return buyMax();
    case BuyAvg:
      return buyAvg();
    case BuyMedian:
      return buyMedian();
    case BuyStdDev:
      return buyStdDev();
    case User:
      return userPrice();
    default:
      return -2.;
  }
}

long PriceInfo::sellVolume() const
{
  return sellVolume_;
}

long PriceInfo::buyVolume() const
{
  return buyVolume_;
}

void PriceInfo::setSellAvg(double sellAvg)
{
  sellAvg_ = sellAvg;
}

void PriceInfo::setSellMin(double sellMin)
{
  sellMin_ = sellMin;
}

void PriceInfo::setSellMax(double sellMax)
{
  sellMax_ = sellMax;
}

void PriceInfo::setSellStdDev(double sellStdDev)
{
  sellStdDev_ = sellStdDev;
}

void PriceInfo::setSellMedian(double sellMedian)
{
  sellMedian_ = sellMedian;
}

void PriceInfo::setBuyAvg(double buyAvg)
{
  buyAvg_ = buyAvg;
}

void PriceInfo::setBuyMin(double buyMin)
{
  buyMin_ = buyMin;
}

void PriceInfo::setBuyMax(double buyMax)
{
  buyMax_ = buyMax;
}

void PriceInfo::setBuyStdDev(double buyStdDev)
{
  buyStdDev_ = buyStdDev;
}

void PriceInfo::setBuyMedian(double buyMedian)
{
  buyMedian_ = buyMedian;
}

void PriceInfo::setSellVolume(long sellVolume)
{
  sellVolume_ = sellVolume;
}

void PriceInfo::setBuyVolume(long buyVolume)
{
  buyVolume_ = buyVolume;
}

void PriceInfo::setInitialized(bool initialized)
{
  initialized_ = initialized;
}

int PriceInfo::typeID() const
{
  return typeID_;
}

void PriceInfo::addSellOrder(double price, long quantity)
{
  addSellOrder({quantity, price});
}

void PriceInfo::addSellOrder(PriceInfo::Order order)
{
  sellOrders_.push_back(order);
}

void PriceInfo::addBuyOrder(double price, long quantity)
{
  addBuyOrder({quantity, price});
}

void PriceInfo::addBuyOrder(PriceInfo::Order order)
{
  buyOrders_.push_back(order);
}

void PriceInfo::updatePrices()
{
  if (!sellOrders_.isEmpty()) {
    std::sort(sellOrders_.begin(), sellOrders_.end(), [](const Order & left, const Order & right) {
      return left.price < right.price;
    });
    sellMin_ = sellOrders_.first().price;
    sellMax_ = sellOrders_.last().price;

    double sum = std::accumulate(sellOrders_.begin(), sellOrders_.end(), 0., [](double sum, const Order & order) {
      return sum + order.price;
    });

    sellAvg_ = sum / sellOrders_.size();
    sellMedian_ = sellOrders_[sellOrders_.size() / 2 + sellOrders_.size() % 2].price;
  }

  if (!buyOrders_.isEmpty()) {
    std::sort(buyOrders_.begin(), buyOrders_.end(), [](const Order & left, const Order & right) {
      return left.price > right.price;
    });

    buyMax_ = buyOrders_.first().price;
    buyMin_ = buyOrders_.last().price;

    double sum = std::accumulate(buyOrders_.begin(), buyOrders_.end(), 0., [](double sum, const Order & order) {
      return sum + order.price;
    });

    buyAvg_ = sum / buyOrders_.size();
    buyMedian_ = buyOrders_[buyOrders_.size() / 2 + buyOrders_.size() % 2].price;
  }

  initialized_ = true;
  emit updated();
}

class spacePunct : public std::numpunct<char>
{
  protected:
    virtual char do_thousands_sep() const override {return ' ';} //TODO replace space with \u2009(thin space)
    virtual std::string do_grouping() const override {return std::string("\3");}
};

QString PriceInfo::formatPrice(double price, int prec, const QString& currency)
{
  if (price < 0)
    return QChar(0x21BA);

  std::ostringstream os;
  os.imbue(std::locale(os.getloc(), new spacePunct));
  os.precision(prec);
  std::fixed(os);

  os << price;
  QString formatted = QString::fromStdString(os.str());// + " " + currency;

  return formatted;
}

double PriceInfo::userPrice() const
{
  return userPrice_;
}

void PriceInfo::setUserPrice(double userPrice)
{
  userPrice_ = userPrice;
}

int PriceInfo::getPriceType() const
{
  return priceType;
}

void PriceInfo::setPriceType(int value)
{
  priceType = value;
}

bool PriceInfo::initialized() const
{
  return initialized_;
}
