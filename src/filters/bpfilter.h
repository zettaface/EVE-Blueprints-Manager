#ifndef BPFILTER_H
#define BPFILTER_H

#include <QSharedPointer>
#include <QDataStream>
#include <QJsonObject>

#include <yaml-cpp/yaml.h>

#include "../blueprint.h"

class BpFilter : public QObject
{
    Q_OBJECT
  public:
    using Value = QPair<int, int>;

    BpFilter() {}
    BpFilter(const QVariant& val) : val_(val) {}
    virtual ~BpFilter() {}

    virtual bool validate(const QModelIndex& index) const;
    virtual bool validate(const Blueprint& /*bp*/) const { return true; }
    virtual int save(int startID, int parentID) const;
    virtual void addChild(QSharedPointer<BpFilter> /*f*/) {}
    virtual const QVector<QSharedPointer<BpFilter>>& childs() const { return childs_; }

    QVariant value() const;
    void setValue(const QVariant& val);

    int type() const;
    void setType(int type);

    virtual QString displayData() const = 0;
    virtual QJsonObject saveToJSON() const = 0;
    virtual YAML::Node saveToYaml(YAML::Node&) const = 0;
    virtual void loadFromJSON(const QJsonObject&) = 0;
    virtual void loadFromYaml(const YAML::Node&) = 0;

  signals:
    void valueChanged();

  protected:
    int type_;
    QVector<QSharedPointer<BpFilter>> childs_;
    QVariant val_;
};

QDataStream& operator<<(QDataStream& out, const BpFilter::Value& val);
QDataStream& operator>>(QDataStream& in, BpFilter::Value& val);

QDebug operator<<(QDebug out, const BpFilter& bc);

#endif // BPFILTER_H
