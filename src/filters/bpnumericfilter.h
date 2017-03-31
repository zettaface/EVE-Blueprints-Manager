#ifndef BPNUMERICFILTER_H
#define BPNUMERICFILTER_H

#include "bpfilter.h"

class BpNumericFilter : public BpFilter
{
    using Value = QPair<int, int>;
  public:
    BpNumericFilter(int column) : column_{column} {}

    enum Type
    {
      LESS = 0,
      LESS_EQUAL,
      GREATER,
      GREATER_EQUAL,
      EQUAL,
      NOT_EQUAL,
      FuncsCount
    };

    bool validate(const Blueprint& bp) const override;
    bool compare(int val1, int val2, int func) const;
    QString displayData() const override;

    QJsonObject saveToJSON() const override;
    YAML::Node saveToYaml(YAML::Node& node) const override;
    void loadFromJSON(const QJsonObject&) override;
    void loadFromYaml(const YAML::Node&) override;
  private:
    int column_ {0};
};

#endif // BPNUMERICFILTER_H
