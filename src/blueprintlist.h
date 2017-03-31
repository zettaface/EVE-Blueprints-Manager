#ifndef BLUEPRINTLIST_H
#define BLUEPRINTLIST_H

#include <QObject>
#include <QAbstractItemModel>

#include "eve/api.h"
#include "blueprint.h"

class BlueprintList : public QAbstractItemModel
{
    Q_OBJECT
  public:
    explicit BlueprintList(QObject* parent = 0);
    ~BlueprintList() = default;

    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool hasChildren(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    const Blueprint& blueprint(int hid, int row) const;
    const Blueprint& blueprint(const QModelIndex& index) const;
    const BlueprintHeader& blueprintHeader(int hid) const;
    bool hasGroupColumn(int column) const;

    enum DataRoles
    {
      RawData = Qt::UserRole,
      CategoryID = Qt::UserRole + 1,
      GroupID,
      BlueprintRole,
      BlueprintHeaderRole,
      IsHeaderRole
    };

    bool isFlat() const;
    bool showAll() const;
    QSet<int> groupColumns() const;
    void setGroupColumns(const QSet<int>& groupColumns);

  public slots:
    void setShowAll(bool showAll);
    void setFlat(bool flat);
    void loadFromMemory();
    void addGroupColumn(int column);
    void removeGroupColumn(int column);
  signals:
    void modelFetched();
    void requestBPFiltersUpdate();
    void startProcessing();
    void finishProcessing();
  private:
    QVariant userRoleData(const QModelIndex& index) const;
    QStringList makeSelectList(const QMap<int, QString>& names) const;
    QString makeFetchQuery(bool showAll);

    QVector<BlueprintHeader> headers;
    QString dbConnectionName;

    QSet<int> groupColumns_;
    QMap<int, QString> tableBpColumns;
    QMap<int, QString> tableBpGroups;
    QMap<int, QString> tableUnionColumns_;

    bool flat_ {false};
    bool showAll_ {false};
};

#endif // BLUEPRINTLIST_H
