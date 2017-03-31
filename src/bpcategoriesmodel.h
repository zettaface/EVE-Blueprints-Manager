#ifndef BPCATEGORIESMODEL_H
#define BPCATEGORIESMODEL_H

#include <QAbstractItemModel>
#include <QVector>

namespace eve {

struct invCategory
{
  int ID;
  QString name;
};

class BPCategoriesModel : public QAbstractListModel
{
    Q_OBJECT
  public:
    BPCategoriesModel(QObject* parent = 0);
    int rowCount(const QModelIndex& parent) const;
    int rowByCategoryId(int ID) const;
    QVariant data(const QModelIndex& index, int role) const;

    bool isEmpty() const { return rowCount(QModelIndex()) == 0; }

    invCategory categoryByID(int ID) const;

    enum DataRole
    {
      CategoryID = Qt::UserRole
    };
  private:
    invCategory allCategory;
    static QVector<invCategory> data_;

    friend class StaticData;
};

}

#endif // BPCATEGORIESMODEL_H
