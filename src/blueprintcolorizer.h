#ifndef BLUEPRINTCOLORIZER_H
#define BLUEPRINTCOLORIZER_H

#include <QColor>
#include <QSharedPointer>

#include <functional>
#include <tr1/functional>
#include <unordered_map>

#include "blueprint.h"
#include "filters/blueprintfilters.h"

class BlueprintColorizer
{
    using FUNC = std::function<bool(int, int)>;
  public:
    BlueprintColorizer();

    bool isValidFor(const Blueprint&) const;
    bool hasValidGroup(const Blueprint&) const;
    int distance(const Blueprint&) const;

    QColor fgColor { Qt::black };
    QColor bgColor { Qt::white };
    int groupID { -1 };
    int categoryID { -1 };

    int typeID { 0 };
    int funcID { 0 };
    int valueID { 0 };
    int value { 0 };
    int priority { 0 };

    enum Func
    {
      LESS = 0,
      LESS_EQUAL,
      GREATER,
      GREATER_EQUAL,
      EQUAL,
      FuncsCount
    };

    enum ValueType
    {
      Runs = 0,
      Quantity,
      ValueTypesCount
    };

    enum Type
    {
      Category = 0,
      MarketGroup
    };

    bool compare(int val) const;
    bool compare(const Blueprint&) const;
    int getBpValue(const Blueprint&) const;

    QSharedPointer<BpFilter> filter() const;
    void setFilter(QSharedPointer<BpFilter> filter);

  private:
    QSharedPointer<BpFilter> filter_;

    static const std::unordered_map<int, FUNC> _funcs;
};

QDataStream& operator<<(QDataStream& out, const BlueprintColorizer& bc);
QDataStream& operator>>(QDataStream& in, BlueprintColorizer& bc);

QDebug operator<<(QDebug out, const BlueprintColorizer& bc);


#endif // BLUEPRINTCOLORIZER_H
