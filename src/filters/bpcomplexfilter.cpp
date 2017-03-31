#include "bpcomplexfilter.h"

bool BpComplexFilter::validate(const QModelIndex& bp) const
{
  int type = val_.toInt();
  bool result;

  switch (type) {
    case LOGICAL_AND:
      result = iterate(bp, false, false, true, std::logical_and<bool>());
      break;
    case LOGICAL_OR:
      result = iterate(bp, true, true, false, std::logical_or<bool>());
      break;
    case LOGICAL_NOT:
      result = iterate(bp, false, false, false, [](bool val1, bool val2) -> bool { return (!val1) && (!val2); });
      break;
    default:
      result = false;
      break;
  }

  return result;
}

void BpComplexFilter::insertFilter(QSharedPointer<BpFilter> bp)
{
  childs_.push_back(bp);
  connect(bp.data(), SIGNAL(valueChanged()), this, SIGNAL(valueChanged()));
}

const QVector<QSharedPointer<BpFilter>>& BpComplexFilter::filters() const
{
  return childs_;
}

void BpComplexFilter::swapFilter(QSharedPointer<BpFilter> oldFilter, QSharedPointer<BpFilter> newFilter)
{
  int pos = childs_.indexOf(oldFilter);
  disconnect(oldFilter.data(), 0, this, 0);

  childs_[pos] = newFilter;

  connect(newFilter.data(), SIGNAL(valueChanged()), this, SIGNAL(valueChanged()));
}

void BpComplexFilter::removeFilter(QSharedPointer<BpFilter> filter)
{
  childs_.removeAll(filter);
  disconnect(filter.data(), 0, this, 0);

  emit valueChanged();
}
int BpComplexFilter::save(int startID, int parentID) const
{
  int myId = BpFilter::save(startID, parentID);
  int currentID = myId;
  for (auto f : childs_)
    f->save(currentID++, myId);

  return currentID;
}

QString BpComplexFilter::displayData() const
{
  static const QMap<int, QString> functorNames {
    { LOGICAL_AND, ", " },
    { LOGICAL_OR, " <b>OR</b> " },
    { LOGICAL_NOT, " <b>NOT</b> " }
  };

  if (childs_.size() == 0)
    return "Any";

  QString data = "(" + childs_[0]->displayData();
  for (int i = 1; i < childs_.size(); i++)
    data += QString("%1%2").arg(functorNames.value(val_.toInt(), "???"), childs_[i]->displayData());

  data += ")";
  return data;
}


QJsonObject BpComplexFilter::saveToJSON() const
{
  QJsonObject json;
  json["operator"] = val_.toInt();

  return json;
}

void BpComplexFilter::loadFromJSON(const QJsonObject& json)
{
  setValue(json["operator"].toInt());
}


YAML::Node BpComplexFilter::saveToYaml(YAML::Node& node) const
{
  node["operator"] = val_.toInt();

  return node;
}

void BpComplexFilter::loadFromYaml(const YAML::Node& node)
{
  setValue(node["operator"].as<int>());
}
