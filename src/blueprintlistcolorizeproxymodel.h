#ifndef BLUEPRINTLISTCOLORIZEPROXYMODEL_H
#define BLUEPRINTLISTCOLORIZEPROXYMODEL_H

#include <QObject>
#include <QHash>
#include <QIdentityProxyModel>

#include "bpcolorsmodel.h"
#include "blueprintlist.h"

inline uint qHash(const QPair<int, int>& p, uint seed)
{
  return qHash(p.first, seed) ^ p.second;
}

class BlueprintListColorizeProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
  public:
    explicit BlueprintListColorizeProxyModel(QObject* parent = 0);

    void setSourceModel(QAbstractItemModel* sourceModel) override;
    QVariant data(const QModelIndex& index, int role) const override;

    BPListColorModel* bpColors() const;
    void setBpColors(BPListColorModel* bpColors);

  public slots:
    void clearCache();

  private:
    BlueprintColorizer findColorizer(const QModelIndex&) const;
    BlueprintColorizer colorizer(const QModelIndex&) const;
    BPListColorModel* bpColorsModel_ {nullptr};
    mutable QHash<QPair<int, int>, BlueprintColorizer> bpColorsCache_;

  private slots:
    void clearCache(const QModelIndex&, const QModelIndex&);
};

#endif // BLUEPRINTLISTCOLORIZEPROXYMODEL_H
