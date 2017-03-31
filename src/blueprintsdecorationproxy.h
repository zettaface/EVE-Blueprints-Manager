#ifndef BLUEPRINTSDECORATIONPROXY_H
#define BLUEPRINTSDECORATIONPROXY_H

#include <QHash>
#include <QIdentityProxyModel>
#include <QObject>
#include <QPixmap>


class BlueprintsDecorationProxy : public QIdentityProxyModel
{
    Q_OBJECT
  public:
    explicit BlueprintsDecorationProxy(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role) const override;
    void setSourceModel(QAbstractItemModel* sourceModel) override;
  public slots:
    void clearCache();
  private:
    QPixmap makeDecoration(const QModelIndex&) const;
    void initActivityIcons();

    mutable QHash<int, QPixmap> decorations;
    QMap<int, QPixmap> activityIcons;
};

#endif // BLUEPRINTSDECORATIONPROXY_H
