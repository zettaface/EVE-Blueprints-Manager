#ifndef BPMARKETGROUPFILTER_H
#define BPMARKETGROUPFILTER_H

#include "bpfilter.h"

class BpMarketGroupFilter : public BpFilter
{
  public:
    bool validate(const Blueprint& bp) const override;
    QString displayData() const override;

    QJsonObject saveToJSON() const override;
    YAML::Node saveToYaml(YAML::Node& node) const override;
    void loadFromJSON(const QJsonObject&) override;
    void loadFromYaml(const YAML::Node&) override;
};

#endif // BPMARKETGROUPFILTER_H
