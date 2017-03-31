#include "bpactivityfilter.h"

#include "../eve/staticdata.h"

bool BpActivityFilter::validate(const Blueprint& bp) const
{
  int activity = val_.toInt();
  if (activity == -1 && bp.activity > 0)
    return true;

  return activity == bp.activity;
}

QString BpActivityFilter::displayData() const
{
  int activity = val_.toInt();
  if (activity == -1)
    return "Any activity";

  return QString("Activity is <u>%1</u>").arg(eve::SDE.industryActivityModel()->activityName(activity));
}


QJsonObject BpActivityFilter::saveToJSON() const
{
  QJsonObject json;
  json["activityType"] = val_.toInt();

  return json;
}

void BpActivityFilter::loadFromJSON(const QJsonObject& json)
{
  setValue(json["activityType"].toInt());
}


YAML::Node BpActivityFilter::saveToYaml(YAML::Node& node) const
{
  node["activityType"] = val_.toInt();

  return node;
}

void BpActivityFilter::loadFromYaml(const YAML::Node& node)
{
  setValue(node["activityType"].as<int>());
}
