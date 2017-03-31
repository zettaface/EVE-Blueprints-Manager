#ifndef BPFILTERFACTORY_H
#define BPFILTERFACTORY_H

#include <QWidget>
#include <QJsonObject>

#include <yaml-cpp/yaml.h>

#include "abstractfactory.h"
#include "filters/blueprintfilters.h"
#include "filterEditors/filtereditors.h"

class BpFilterFactory
{
  public:
    enum Type
    {
      None = 0,
      CategoryGroup,
      MarketGroup,
      Name,
      Runs,
      Quantity,
      ME,
      TE,
      Logical,
      Activity,
      IsHeader,
      MetaLevel,
      Owner,
      Location,
      TypeCount
    };

    BpFilterFactory();
    virtual ~BpFilterFactory() {}

    template<typename F, typename... Args>
    void registerFilter(int type, const QString& name, Args... args) {
      filterFactory_.Register<F>(type, args...);
      registeredFilters.insert(type);
      filterNames_.insert(type, name);
    }

    template<typename E, typename... Args>
    void registerFilterEditor(int type, Args... args) {
      filterEditorFactory_.Register<E>(type, args...);
    }

    QString filterName(int type) const;

    QSharedPointer<BpFilter> createFilter(int type) const;
    QWidget* createFilterEditor(QSharedPointer<BpFilter> filter, QWidget* parent = nullptr) const;
    QWidget* createFilterEditor(int type, QWidget* parent = nullptr) const;

    static QSharedPointer<BpFilter> loadFilters(const QJsonObject& json);
    static QSharedPointer<BpFilter> loadFilters(const YAML::Node& yaml);

    static QJsonObject saveFiltersAsJson(QSharedPointer<BpFilter> root);
    static YAML::Node saveFiltersAsYaml(QSharedPointer<BpFilter> root);

    inline QSet<int> availableFilters() const { return registeredFilters; }

    static BpFilterFactory& instance() {
      static BpFilterFactory instance_;
      return instance_;
    }
  private:
    Factory<BpFilter> filterFactory_;
    Factory<BpFilterEditor> filterEditorFactory_;
    QMap<int, QString> filterNames_;

    QSet<int> registeredFilters;

};

QDataStream& operator<<(QDataStream& out, const QSharedPointer<BpFilter>& bc);
QDataStream& operator>>(QDataStream& in, QSharedPointer<BpFilter>& bc);


#endif // BPFILTERFACTORY_H
