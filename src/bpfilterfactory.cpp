#include "bpfilterfactory.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDataStream>

#include <yaml-cpp/yaml.h>

#include "blueprintsfilterwidget.h"
#include "filterEditors/bpfiltereditor.h"

BpFilterFactory::BpFilterFactory()
{
  registerFilter<BpCategoryGroupFilter>(CategoryGroup, "Category/Group");
  registerFilter<BpMarketGroupFilter>(MarketGroup, "Market group");
  registerFilter<BpStringFilter>(Name, "Name", Blueprint::Name);
  registerFilter<BpNumericFilter>(Runs, "Runs", Blueprint::Runs);
  registerFilter<BpNumericFilter>(Quantity, "Quantity", Blueprint::Quantity);
  registerFilter<BpNumericFilter>(ME, "ME", Blueprint::MEc);
  registerFilter<BpNumericFilter>(TE, "TE", Blueprint::TEc);
  registerFilter<BpComplexFilter>(Logical, "Logical");
  registerFilter<BpActivityFilter>(Activity, "Activity");
  registerFilter<BpIsHeaderFilter>(IsHeader, "Is group");
  registerFilter<BpNumericFilter>(MetaLevel, "Meta lvl", Blueprint::MetaLevel);
  registerFilter<BpStringFilter>(Owner, "Owner", Blueprint::Owner);
  registerFilter<BpStringFilter>(Location, "Location", Blueprint::LocationIDc);

  registerFilterEditor<CategoryGroupFilterEditor>(CategoryGroup);
  registerFilterEditor<MarketGroupFilterEditor>(MarketGroup);
  registerFilterEditor<SingleLineEditor>(Name);
  registerFilterEditor<ComboLineEditor>(Runs, BpNumericFilter::GREATER_EQUAL, "-1");
  registerFilterEditor<ComboLineEditor>(Quantity, BpNumericFilter::GREATER_EQUAL, "1");
  registerFilterEditor<ComboLineEditor>(ME, BpNumericFilter::GREATER_EQUAL, "0");
  registerFilterEditor<ComboLineEditor>(TE, BpNumericFilter::GREATER_EQUAL, "0");
  registerFilterEditor<ComplexEditor>(Logical);
  registerFilterEditor<ActivityFilterEditor>(Activity);
  registerFilterEditor<IsHeaderFilterEditor>(IsHeader);
  registerFilterEditor<ComboLineEditor>(MetaLevel, BpNumericFilter::GREATER_EQUAL, "0");
  registerFilterEditor<SingleLineEditor>(Owner);
  registerFilterEditor<SingleLineEditor>(Location);
}

QString BpFilterFactory::filterName(int type) const
{
  return filterNames_.value(type, "Unknown filter");
}

QSharedPointer<BpFilter> BpFilterFactory::createFilter(int type) const
{
  Q_ASSERT_X(type < TypeCount, "Filter Factory", "Filter type out of range");

  QSharedPointer<BpFilter> filter = filterFactory_.create<QSharedPointer>(static_cast<int>(type));
  filter->setType(type);
  return filter;
}

QWidget* BpFilterFactory::createFilterEditor(QSharedPointer<BpFilter> filter, QWidget* parent) const
{
  return createFilterEditor(filter->type(), parent);
}

QWidget* BpFilterFactory::createFilterEditor(int type, QWidget* parent) const
{
  Q_ASSERT_X(type < TypeCount, "Filter Factory", "Editor type out of range");

  BpFilterEditor* editor = filterEditorFactory_.create(type);
  editor->setFilter(createFilter(type));

  return editor;
}

QSharedPointer<BpFilter> BpFilterFactory::loadFilters(const QJsonObject& json)
{
  BpFilterFactory bf;

  if (json.isEmpty())
    return QSharedPointer<BpFilter>();

  int type = json["type"].toInt();
  QSharedPointer<BpFilter> filter = bf.createFilter(type);
  filter->loadFromJSON(json["value"].toObject());

  if (type != Logical)
    return filter;

  QJsonArray childs = json["childs"].toArray();
  for (int i = 0; i < childs.size(); i++)
    filter->addChild(loadFilters(childs[i].toObject()));

  return filter;
}

QSharedPointer<BpFilter> BpFilterFactory::loadFilters(const YAML::Node& yaml)
{
  BpFilterFactory bf;
  if (yaml.IsNull())
    return QSharedPointer<BpFilter>();

  int type = yaml["type"].as<int>();
  QSharedPointer<BpFilter> filter = bf.createFilter(type);
  filter->loadFromYaml(yaml);

  if (type != Logical)
    return filter;

  const YAML::Node& childs = yaml["childs"];
  for (int i = 0; i < childs.size(); i++)
    filter->addChild(loadFilters(childs[i]));

  return filter;
}

QJsonObject BpFilterFactory::saveFiltersAsJson(QSharedPointer<BpFilter> root)
{
  QJsonObject json;
  json["type"] = root->type();
  json["value"] = root->saveToJSON();

  QJsonArray array;
  const QVector<QSharedPointer<BpFilter>>& childs { root->childs() };
  for (int i = 0; i < childs.size(); i++)
    array.push_back(saveFiltersAsJson(childs[i]));

  if (!array.isEmpty())
    json["childs"] = array;

  return json;
}

YAML::Node BpFilterFactory::saveFiltersAsYaml(QSharedPointer<BpFilter> root)
{
  YAML::Node node;
  node["type"] = root->type();
  root->saveToYaml(node);

  const QVector<QSharedPointer<BpFilter>>& childs { root->childs() };
  for (int i = 0; i < childs.size(); i++)
    node["childs"].push_back(saveFiltersAsYaml(childs[i]));

  return node;
}

QDataStream& operator<<(QDataStream& out, const QSharedPointer<BpFilter>& bc)
{
  QJsonDocument json(BpFilterFactory::saveFiltersAsJson(bc));
  out << json.toJson(QJsonDocument::Compact);
  return out;
}

QDataStream& operator>>(QDataStream& in, QSharedPointer<BpFilter>& bc)
{
  QByteArray raw;
  in >> raw;
  QJsonDocument json = QJsonDocument::fromJson(raw);
  bc = BpFilterFactory::loadFilters(json.object());
  return in;
}
