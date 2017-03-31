#include "bpmarketgroupfilter.h"

#include "../eve/staticdata.h"

bool BpMarketGroupFilter::validate(const Blueprint& bp) const
{
  return bp.prodMarketGroups.contains(val_.toInt());
}

QString BpMarketGroupFilter::displayData() const
{
  if (val_.toInt() < 0)
    return QString("Any market group");

  return QString("Market group is <u>%1</u>").arg(eve::SDE.marketGroupName(val_.toInt()));
}


QJsonObject BpMarketGroupFilter::saveToJSON() const
{
  QJsonObject json;
  json["marketGroupID"] = val_.toInt();

  return json;
}

void BpMarketGroupFilter::loadFromJSON(const QJsonObject& json)
{
  setValue(json["marketGroupID"].toInt());
}


YAML::Node BpMarketGroupFilter::saveToYaml(YAML::Node& node) const
{
  node["marketGroupID"] = val_.toInt();

  return node;

}

void BpMarketGroupFilter::loadFromYaml(const YAML::Node& node)
{
  setValue(node["marketGroupID"].as<int>());
}
