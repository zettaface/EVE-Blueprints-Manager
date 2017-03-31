#ifndef BLUEPRINTSAGGREGATIONPROXY_H
#define BLUEPRINTSAGGREGATIONPROXY_H

#include <QIdentityProxyModel>
#include <QMap>

class BlueprintsAggregationProxy : public QIdentityProxyModel
{
    Q_OBJECT
  public:
    explicit BlueprintsAggregationProxy(QObject* parent = nullptr) : QIdentityProxyModel(parent) {}

    QVariant data(const QModelIndex& index, int role) const override;
    long totalRuns() const;
    long totatQuantity() const;

  public slots:
    void clearCache();

  private:
    struct AggregationInfo
    {
      int runs {0};
      int quantity {0};
      bool hasOriginal {false};
    };

    QVariant formatRunsData(int runs) const;
    void calcStats() const;
    void gatherAggregationData() const;

    mutable long totalRuns_ {-1};
    mutable long totalQuantity_ {-1};
    mutable QVector<AggregationInfo> data_;
};

#endif // BLUEPRINTSAGGREGATIONPROXY_H
