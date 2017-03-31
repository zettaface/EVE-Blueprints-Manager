#ifndef BPACTIVITYFILTER_H
#define BPACTIVITYFILTER_H

#include "bpfilter.h"

class BpActivityFilter : public BpFilter
{
  public:
    bool validate(const Blueprint& bp) const override;
    QString displayData() const override;

    QJsonObject saveToJSON() const override;
    YAML::Node saveToYaml(YAML::Node& node) const override;

    void loadFromJSON(const QJsonObject&) override;
    void loadFromYaml(const YAML::Node&) override;
};

#endif // BPACTIVITYFILTER_H
