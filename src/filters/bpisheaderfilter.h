#ifndef BPISHEADERFILTER_H
#define BPISHEADERFILTER_H

#include "bpfilter.h"

class BpIsHeaderFilter : public BpFilter
{
    Q_OBJECT
  public:
    bool validate(const QModelIndex& index) const override;
    QString displayData() const override;

    QJsonObject saveToJSON() const override;
    YAML::Node saveToYaml(YAML::Node& node) const override;
    void loadFromJSON(const QJsonObject&) override;
    void loadFromYaml(const YAML::Node&) override;
};

#endif // BPISHEADERFILTER_H
