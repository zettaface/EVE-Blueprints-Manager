#include "bpcategorygroupfilter.h"

#include "../eve/staticdata.h"

bool BpCategoryGroupFilter::validate(const Blueprint& bp) const
{
  QPair<int, int> tmp = val_.value<Value>();
  int categoryID = tmp.first;
  int groupID    = tmp.second;

  if (categoryID <= 0)
    return true;

  if (categoryID == bp.prodCategoryID) {
    if (groupID <= 0 || groupID == bp.prodGroupID)
      return true;
  }

  return false;
}

QString BpCategoryGroupFilter::displayData() const
{
  QPair<int, int> tmp = val_.value<Value>();
  int categoryID = tmp.first;
  int groupID    = tmp.second;

  if (categoryID < 0)
    return QString("Any category/group");

  if (groupID < 0)
    return QString("Category is %1").arg(categoryID);

  return QString("Category is <u>%1</u>, group is <u>%2</u>").arg(eve::SDE.categoryName(categoryID)).arg(eve::SDE.groupName(groupID));
}


QJsonObject BpCategoryGroupFilter::saveToJSON() const
{
  QPair<int, int> tmp = val_.value<Value>();
  int categoryID = tmp.first;
  int groupID    = tmp.second;

  QJsonObject json;
  json["category"] = categoryID;
  json["group"] = groupID;

  return json;
}

void BpCategoryGroupFilter::loadFromJSON(const QJsonObject& json)
{
  int categoryID = json["category"].toInt();
  int groupID    = json["group"].toInt();

  QPair<int, int> tmp {categoryID, groupID};
  setValue(QVariant::fromValue<QPair<int, int>>(tmp));
}


YAML::Node BpCategoryGroupFilter::saveToYaml(YAML::Node& node) const
{
  QPair<int, int> tmp = val_.value<Value>();
  int categoryID = tmp.first;
  int groupID    = tmp.second;

  node["category"] = categoryID;
  node["group"] = groupID;

  return node;

}

void BpCategoryGroupFilter::loadFromYaml(const YAML::Node& node)
{
  int categoryID = node["category"].as<int>();
  int groupID    = node["group"].as<int>();

  QPair<int, int> tmp {categoryID, groupID};
  setValue(QVariant::fromValue<QPair<int, int>>(tmp));
}
