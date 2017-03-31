#include "blueprintcolorizer.h"

#include <QDebug>
#include <QDataStream>
#include <algorithm>

#include "bpfilterfactory.h"
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include "blueprintsfilterboxwidget.h"

BlueprintColorizer::BlueprintColorizer()
{
  BpFilterFactory factory;
  filter_ = factory.createFilter(BpFilterFactory::Logical);
}

const std::unordered_map<int, BlueprintColorizer::FUNC> BlueprintColorizer::_funcs = {
  {BlueprintColorizer::Func::LESS, std::less<int>()},
  {BlueprintColorizer::Func::LESS_EQUAL, std::less_equal<int>()},
  {BlueprintColorizer::Func::GREATER, std::greater<int>()},
  {BlueprintColorizer::Func::GREATER_EQUAL, std::greater_equal<int>()},
  {BlueprintColorizer::Func::EQUAL, std::equal_to<int>()}
};

QSharedPointer<BpFilter> BlueprintColorizer::filter() const
{
  return filter_;
}

void BlueprintColorizer::setFilter(QSharedPointer<BpFilter> filter)
{
  filter_ = filter;
}

QDataStream& operator<<(QDataStream& out, const BlueprintColorizer& bc)
{
  out << bc.fgColor
      << bc.bgColor
      << bc.filter()
      << bc.priority;
  return out;
}

QDataStream& operator>>(QDataStream& in, BlueprintColorizer& bc)
{
  QSharedPointer<BpFilter> filter;
  in >> bc.fgColor >> bc.bgColor >> filter >> bc.priority;
  bc.setFilter(filter);

  return in;
}

QDebug operator<<(QDebug out, const BlueprintColorizer& bc)
{
  out << "Colorizer("
      << bc.fgColor.name()
      << ", "
      << bc.bgColor.name()
      << ", "
      << bc.filter()
      << ", "
      << bc.priority
      << ")";

  return out;
}
