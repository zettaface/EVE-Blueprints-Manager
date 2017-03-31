#ifndef MARKETGROUPSMODEL_H
#define MARKETGROUPSMODEL_H

#include <QVector>
#include <QSet>
#include <QString>
#include <QAbstractItemModel>

struct MarketGroupNode
{
  int marketGroupID;
  int parentGroup;
  QString name;
  QString description;
  QVector<int> childs;
};

class MarketGroupsModel : public QAbstractItemModel
{
    Q_OBJECT
    using NodeList = QVector<int>;
  public:
    MarketGroupsModel(QObject* parent = 0) : QAbstractItemModel(parent) {}

    void fetchFromSDE();

    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex index(int groupID, int column) const;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant data(int groupID, int column, int role = Qt::DisplayRole) const;
    bool hasChildren(const QModelIndex& parent) const override;
    int parentsCount(int) const;
    QSet<int> getParentGroups(int) const;
    int groupDepth(int groupID) const;

    enum Columns
    {
      Ramification = 0,
      Name = Ramification,
      GroupID,
      Description,
      ColumnCount
    };

  private:
    int findRow(const MarketGroupNode& node) const;
    QString groupPath(const MarketGroupNode& node, const QString& currentPath = "") const;
    QVariant displayRoleData(const QModelIndex& index) const;
    QVariant userData(const QModelIndex& index) const;

    QMap<int, MarketGroupNode> _tree;
};

#endif // MARKETGROUPSMODEL_H
