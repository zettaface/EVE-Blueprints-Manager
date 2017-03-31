#ifndef CATEGORYFILTERPROXYMODEL_H
#define CATEGORYFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>


class CategoryFilterProxyModel : public QSortFilterProxyModel
{
  public:
    CategoryFilterProxyModel(QObject* parent = 0) : QSortFilterProxyModel(parent) {}
    ~CategoryFilterProxyModel() = default;

    int categoryID() const;
    void setCategoryID(int ID);
    bool isAllShowed() const;
    void setShowAll(bool all);

  protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

    int _categoryID;
    bool _all { true } ;
};

#endif // CATEGORYFILTERPROXYMODEL_H
