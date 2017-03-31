#include "bpnumericfilter.h"

bool BpNumericFilter::validate(const Blueprint& bp) const
{
  QPair<int, int> p = val_.value<QPair<int, int>>();
  return compare(bp.data(column_).toInt(), p.first, p.second);
}

bool BpNumericFilter::compare(int val1, int val2, int func) const
{
  switch(func) {
    case LESS:
      return val1 < val2;
    case LESS_EQUAL:
      return val1 <= val2;
    case GREATER:
      return val1 > val2;
    case GREATER_EQUAL:
      return val1 >= val2;
    case EQUAL:
      return val1 == val2;
    case NOT_EQUAL:
      return val1 != val2;
    default:
      return false;
  }
}

QString BpNumericFilter::displayData() const
{
  static const QMap<int, QString> functorNames {
    {LESS, "&lt;"},
    {LESS_EQUAL, "&lt;="},
    {GREATER, "&gt;"},
    {GREATER_EQUAL, "&gt;="},
    {EQUAL, "="},
    {NOT_EQUAL, "!="}
  };

  QPair<int, int> tmp = val_.value<Value>();
  return QString("%1 %2 %3").arg(Blueprint::headerData(column_).toString()).arg(functorNames.value(tmp.second, "???")).arg(tmp.first);
}


QJsonObject BpNumericFilter::saveToJSON() const
{
  QPair<int, int> tmp = val_.value<Value>();

  QJsonObject json;
  json["operator"] = tmp.second;
  json["value"] = tmp.first;

  return json;
}

void BpNumericFilter::loadFromJSON(const QJsonObject& json)
{
  int op = json["operator"].toInt();
  int val = json["value"].toInt();

  QPair<int, int> tmp {val, op};

  setValue(QVariant::fromValue<Value>(tmp));
}


YAML::Node BpNumericFilter::saveToYaml(YAML::Node& node) const
{
  QPair<int, int> tmp = val_.value<Value>();

  node["operator"] = tmp.second;
  node["value"] = tmp.first;

  return node;
}

void BpNumericFilter::loadFromYaml(const YAML::Node& node)
{
  int op = node["operator"].as<int>();
  int val = node["value"].as<int>();

  QPair<int, int> tmp {val, op};

  setValue(QVariant::fromValue<Value>(tmp));
}
