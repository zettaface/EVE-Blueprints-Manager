#ifndef BPSTRINGFILTER_H
#define BPSTRINGFILTER_H

#include "bpfilter.h"

class BpStringFilter : public BpFilter
{
  public:
    BpStringFilter(int column) : column_(column) {}

    bool validate(const Blueprint& bp) const override;
    QString displayData() const override;

    QJsonObject saveToJSON() const override;
    YAML::Node saveToYaml(YAML::Node& node) const override;
    void loadFromJSON(const QJsonObject&) override;
    void loadFromYaml(const YAML::Node&) override;
  private:
    int column_ {0};
};

#endif // BPSTRINGFILTER_H
