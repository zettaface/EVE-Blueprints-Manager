#ifndef BPCATEGORYGROUPFILTER_H
#define BPCATEGORYGROUPFILTER_H

#include "bpfilter.h"

class BpCategoryGroupFilter : public BpFilter
{
    Q_OBJECT
  public:
    bool validate(const Blueprint& bp) const override;
    QString displayData() const override;

    QJsonObject saveToJSON() const override;
    YAML::Node saveToYaml(YAML::Node& node) const override;
    void loadFromJSON(const QJsonObject&) override;
    void loadFromYaml(const YAML::Node&) override;
};

#endif // BPCATEGORYGROUPFILTER_H
