#include "blueprintssortproxy.h"

#include <QColor>

#include <algorithm>

#include "blueprintlist.h"

BlueprintsSortProxy::BlueprintsSortProxy(QObject* parent) :
  QSortFilterProxyModel(parent),
  type_(Name)
{

}

bool BlueprintsSortProxy::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
  switch (type_) {
    case Name:
      return stringLess(Blueprint::Name, source_left, source_right);
    case Location:
      return stringLess(Blueprint::LocationIDc, source_left, source_right);
    case Runs:
      return numericLess(Blueprint::Runs, source_left, source_right);
    case Quantity:
      return numericLess(Blueprint::Quantity, source_left, source_right);
    case ME:
      return numericLess(Blueprint::MEc, source_left, source_right);
    case TE:
      return numericLess(Blueprint::TEc, source_left, source_right);
    case Color:
      return colorLess(source_left, source_right);
    default:
      return BlueprintsSortProxy::lessThan(source_left, source_right);
  }
}

bool BlueprintsSortProxy::colorLess(const QModelIndex& source_left, const QModelIndex& source_right) const
{
  QColor c1 =  source_left.data(Qt::BackgroundRole).value<QColor>().toHsv();
  QColor c2 = source_right.data(Qt::BackgroundRole).value<QColor>().toHsv();

  auto ct1 = std::make_tuple(c1.hue(), c1.saturation(), c1.value());
  auto ct2 = std::make_tuple(c2.hue(), c2.saturation(), c2.value());

  if (ct1 == ct2)
    return stringLess(Blueprint::Name, source_left, source_right);

  return ct1 < ct2;
}

bool BlueprintsSortProxy::numericLess(int column, const QModelIndex& source_left, const QModelIndex& source_right) const
{
  int left  =  source_left.data(Qt::UserRole).toInt();
  int right = source_right.data(Qt::UserRole).toInt();

  return left < right;
}

bool BlueprintsSortProxy::stringLess(int column, const QModelIndex& source_left, const QModelIndex& source_right) const
{
  QString leftStr  =  source_left.data(Qt::UserRole).toString();
  QString rightStr = source_right.data(Qt::UserRole).toString();

  return QString::localeAwareCompare(leftStr, rightStr) < 0;
}

int BlueprintsSortProxy::type() const
{
  return type_;
}

void BlueprintsSortProxy::setType(int type)
{
  type_ = type;
}
