#ifndef BPCATEGORYGROUPFILTERPROXY_H
#define BPCATEGORYGROUPFILTERPROXY_H

#include <QSortFilterProxyModel>
#include <QColor>
#include <tuple>
#include <algorithm>

class BPCategoryGroupFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    BPCategoryGroupFilterProxy(QObject* parent = 0);
    int categoryID() const;
    void setCategoryID(int value);
    int groupID() const;
    void setGroupID(int groupID);

  protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
    int _categoryID {0};
    int _groupID {0};

  protected:
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override {
      QColor c1 = source_left.data(Qt::BackgroundRole).value<QColor>().toHsv();
      QColor c2 = source_right.data(Qt::BackgroundRole).value<QColor>().toHsv();
      return std::make_tuple(c1.hue(), c1.saturation(), c1.value()) <
             std::make_tuple(c2.hue(), c2.saturation(), c2.value());
    }
};
#endif // BPCATEGORYGROUPFILTER_H
