#include "bpisheaderfilter.h"

#include "../blueprintlist.h"

bool BpIsHeaderFilter::validate(const QModelIndex& index) const
{
  return val_.toBool() == index.data(BlueprintList::IsHeaderRole).toBool();
}


QString BpIsHeaderFilter::displayData() const
{
  bool isHeader = val_.toBool();
  if (isHeader)
    return "Is group";
  else
    return "Is not group";
}


QJsonObject BpIsHeaderFilter::saveToJSON() const
{
  QJsonObject json;
  json["isHeader"] = val_.toBool();

  return json;
}

void BpIsHeaderFilter::loadFromJSON(const QJsonObject& json)
{
  setValue(json["isHeader"].toBool());
}


YAML::Node BpIsHeaderFilter::saveToYaml(YAML::Node& node) const
{
  node["isHeader"] = val_.toBool();

  return node;
}

void BpIsHeaderFilter::loadFromYaml(const YAML::Node& node)
{
  setValue(node["isHeader"].as<bool>());
}
