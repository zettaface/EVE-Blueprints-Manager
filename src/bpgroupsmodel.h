#ifndef BPGROUPSMODEL_H
#define BPGROUPSMODEL_H

#include <QAbstractItemModel>
#include <QVector>

namespace eve {

struct invGroup
{
  int ID;
  int categoryID;
  QString name;

};

class BPGroupsModel : public QAbstractListModel
{
    Q_OBJECT
  public:
    BPGroupsModel(QObject* parent = 0);
    int rowCount(const QModelIndex& parent) const;
    int rowByGroupId(int ID) const;
    QVariant data(const QModelIndex& index, int role) const;

    bool isEmpty() const { return rowCount(QModelIndex()) == 0; }
    invGroup groupByID(int ID) const;

    enum DataRole
    {
      CategoryID = Qt::UserRole,
      GroupID
    };
  private:
    invGroup allGroup;
    static QVector<invGroup> data_;
    friend class StaticData;
};

}

#endif // BPGROUPSMODEL_H
