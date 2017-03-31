#include "bpstringfilter.h"

bool BpStringFilter::validate(const Blueprint& bp) const
{
  return bp.displayData(column_).toString().contains(val_.toString(), Qt::CaseInsensitive);
}

QString BpStringFilter::displayData() const
{
  return QString("%1 contains %2").arg(Blueprint::headerData(column_).toString()).arg(val_.toString());
}

QJsonObject BpStringFilter::saveToJSON() const
{
  QJsonObject json;
  json["string"] = val_.toString();

  return json;
}

void BpStringFilter::loadFromJSON(const QJsonObject& json)
{
  setValue(json["string"].toString());
}


YAML::Node BpStringFilter::saveToYaml(YAML::Node& node) const
{
  node["string"] = val_.toString().toStdString();

  return node;
}

void BpStringFilter::loadFromYaml(const YAML::Node& node)
{
  setValue(QString::fromStdString(node["string"].as<std::string>()));
}
