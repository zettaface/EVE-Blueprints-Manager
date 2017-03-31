#ifndef BPCOMPLEXFILTER_H
#define BPCOMPLEXFILTER_H

#include "bpfilter.h"

class BpComplexFilter : public BpFilter
{
  public:
    BpComplexFilter(int initVal = LOGICAL_AND) : BpFilter(initVal) {
    }

    bool validate(const QModelIndex& bp) const override;
    void insertFilter(QSharedPointer<BpFilter> bp);

    enum Type
    {
      LOGICAL_AND = 0,
      LOGICAL_OR = 1,
      LOGICAL_NOT = 2
    };

    int size() const { return childs_.size(); }
    const QVector<QSharedPointer<BpFilter> >& filters() const;
    void swapFilter(QSharedPointer<BpFilter>, QSharedPointer<BpFilter>);
    void removeFilter(QSharedPointer<BpFilter>);

    void addChild(QSharedPointer<BpFilter> f) {
      insertFilter(f);
      BpFilter::addChild(f);
    }

    QJsonObject saveToJSON() const override;
    YAML::Node saveToYaml(YAML::Node& node) const override;
    void loadFromJSON(const QJsonObject&) override;
    void loadFromYaml(const YAML::Node&) override;
  private:
    template<typename F>
    bool iterate(const QModelIndex& bp, bool cond, bool condVal, bool beginVal, F pred) const {
      if (childs_.isEmpty())
        return true;

      bool init = childs_.first()->validate(bp);
      init = pred(beginVal, init);

      for (int i = 1; i < childs_.size(); i++) {
        QSharedPointer<BpFilter> bf = childs_[i];
        init = pred(init, bf->validate(bp));
        if (init == cond)
          return condVal;
      }
      return init;
    }

    int save(int startID, int parentID) const override;
    QString displayData() const override;
};

#endif // BPCOMPLEXFILTER_H
