#include "productiontype.h"

#include <QSqlDatabase>
#include <QDebug>

#include <algorithm>
#include <locale>
#include <sstream>
#include <iostream>

ProductionType::ProductionType(const Blueprint& bp, ProductionType* parent) :
  productID_(bp.prodTypeID),
  blueprintID_(bp.typeID),
  baseQuantity_(1),
  parent_(parent),
  QObject()
{
  if (bp.ME >= 0)
    ME_ = bp.ME;

  if (bp.TE >= 0)
    TE_ = bp.TE;

  priceType_ = PriceInfo::SellMin;
}

ProductionType::ProductionType(long typeID, long bpID, int quantity, ProductionType* parent) :
  productID_(typeID),
  blueprintID_(bpID),
  baseQuantity_(quantity),
  parent_(parent)
{
  priceType_ = PriceInfo::SellMin;
}

QString ProductionType::name() const
{
  return name_;
}

int ProductionType::baseQuantity() const
{
  return baseQuantity_;
}

int ProductionType::modifiedQuantity() const
{
  float facilityBonus = 1;
  double runs = 1;
  double pME = 0;

  if (parent_) {
    runs = parent_->modifiedQuantity();
    pME = parent_->ME_;
  }

  double materialK = (1. - pME / 100.) * facilityBonus;

  //TODO take into account bpc max runs per copy / bpo
  return std::max(runs, std::ceil(runs * baseQuantity_ * materialK));
}

ProductionType* ProductionType::parent() const
{
  return parent_;
}

void ProductionType::addComponent(ProductionType* p)
{
  connect(p, SIGNAL(priceUpdated(double)), this, SLOT(refreshPrice()));

  components_.push_back(p);
  refreshPrice();
}

ProductionType* ProductionType::component(int row) const
{
  return components_[row];
}

int ProductionType::componentsCount() const
{
  return components_.count();
}

double ProductionType::pricePerItem() const
{
  if (isFinal())
    return priceInfo_->price(priceType_);

  if (!isFinal() && components_.isEmpty())
    return priceInfo_->price(priceType_);

  if (!build_) {
    if (priceInfo_)
      return priceInfo_->price(priceType_);
  } else
    return price_ / modifiedQuantity();

  if (priceInfo_)
    return priceInfo_->price(priceType_);

  return -1.0;
}

double ProductionType::price() const
{
  if (priceInfo_)
    return modifiedQuantity() * pricePerItem();

  qCritical() << "No priceInfo object for" << name_ << productID_ << blueprintID_;
  return -1.0;
}

double ProductionType::buildPrice()
{
  return price_ / modifiedQuantity();
}

double ProductionType::marketPrice()
{
  if (priceInfo_)
    return priceInfo_->price(priceType_);

  qCritical() << "There is no PriceInfo object";

  return -2.0;
}

void ProductionType::refreshPrice()
{
  price_ = 0.;

  for (ProductionType* p : components_)
    price_ += p->price();

  qDebug() << price_;

  emit priceUpdated(price_);
}

int ProductionType::priceType() const
{
  return priceType_;
}

void ProductionType::setPriceType(int priceType)
{
  priceType_ = priceType;
  emit priceUpdated(price());
}

bool ProductionType::isFinal() const
{
  if (blueprintID_ <= 0)
    return true;

  return false;
}

int ProductionType::row() const
{
  if (parent_)
    return parent_->components_.indexOf(const_cast<ProductionType*>(this));

  return 0;
}

int ProductionType::TE() const
{
  return TE_;
}

void ProductionType::setTE(int TE)
{
  TE_ = TE;
}

PriceInfo* ProductionType::priceInfo() const
{
  return priceInfo_;
}

void ProductionType::setPriceInfo(PriceInfo* priceInfo)
{
  priceInfo_ = priceInfo;
  connect(priceInfo, SIGNAL(updated()), this, SLOT(refreshPrice()));

  if (priceInfo->initialized())
    refreshPrice();
}

bool ProductionType::build() const
{
  return build_;
}

void ProductionType::setBuild(bool build)
{
  build_ = build;
  emit priceUpdated(0.);
}

QVariant ProductionType::data(int column) const
{
  switch(column) {
    case MarketPrice:
      return priceInfo_->price(priceType_);
    case BuildPrice:
      return price_ / modifiedQuantity();
    case TotalPrice:
      if (build_)
        return price_;

      return modifiedQuantity() * priceInfo_->price(priceType_);
    default:
      qDebug() << "Wrong column requested";
      return -2;
  }
}



QVariant ProductionType::displayData(int column) const
{
  return formatPrice(data(column).toDouble());

  switch(column) {
    case MarketPrice:
      return formatPrice(priceInfo_->price(priceType_));
    case BuildPrice:
      return formatPrice(price_ / modifiedQuantity());
    case TotalPrice:
      if (build_)
        return formatPrice(price_);

      return formatPrice(modifiedQuantity() * priceInfo_->price(priceType_));
    default:
      qDebug() << "Wrong column requested";
      return -2;
  }

}

QString ProductionType::formatPrice(double price)
{
  return PriceInfo::formatPrice(price);
}

int ProductionType::ME() const
{
  return ME_;
}

void ProductionType::setME(int ME)
{
  ME_ = ME;

  refreshPrice();
}

long ProductionType::productID() const
{
  return productID_;
}

long ProductionType::blueprintID() const
{
  return blueprintID_;
}
