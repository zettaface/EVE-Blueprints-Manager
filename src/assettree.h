#ifndef ASSETTREE_H
#define ASSETTREE_H

#include <QVector>
#include <QString>
#include <QAbstractItemModel>
#include <QNetworkReply>
#include "eve/api.h"
//#include "eve/staticdata.h"

class AssetTree : public QAbstractItemModel
{
    Q_OBJECT
  public:
    AssetTree(QObject* parent = 0);
    ~AssetTree();

    struct AssetNode
    {
      void deleteChilds();
      void setParent(AssetNode*);
      void updateCheckState();
      void checkAll();
      void uncheckAll();
      bool isCheckable();

      QString displayName;
      int ID {-1};
      int keyID { -1};
      int typeID;
      qlonglong location {-1};
      qlonglong itemID {-1};
      int flag {0};
      int quantity {0};

      Qt::CheckState checked {Qt::Unchecked};

      AssetNode* parent {nullptr};
      QVector<AssetNode*> childs;
    };

    QList<QPair<int, qlonglong> > getSelectedIDs() const;

  private:
    using NodeList = QVector<AssetNode*>;
    AssetNode* loadAssetTree(eve::ApiKeyInfo* key);
    void setTreeCheckState(AssetNode* root, const QSet<qlonglong>& activeLocations);
    QList<QPair<int, qlonglong> > getChildNodeSelectedIDs(AssetNode*) const;
    int findRow(const AssetNode*) const;
    void insertNodeByFlag(AssetNode* locRoot);

    QVariant nameColumnData(AssetNode* node, int role) const;
    QVariant quantityColumnData(AssetNode* node, int role) const;
    QVariant itemIDColumnData(AssetNode* node, int role) const;

    enum Columns
    {
      RamificationColumn = 0,
      NameColumn = RamificationColumn,
      QuantityColumn,
      ItemID,
      ColumnCount
    };

    bool fetched {false};
    NodeList locations;
    //EVEApi api;
    QMap<int, QString> hangarFlags;
    bool changed_ { false };

    // QAbstractItemModel interface
    void clearNodes();

  public:
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    bool hasChildren(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void updateParentNodeCheckState(AssetNode* node);
    void updateChildNodesCheckState(AssetNode* node);
    //void fetchMore(const QModelIndex& parent);
    //void fetchAssets();
    //bool fetchAssetsByKey(const EVEApiKeyInfo& key);
//	void insertFilter(AssetNode* node);
//	void removeFilter(AssetNode* node);
    bool isChanged() const;

  signals:
    void selectionChanged();
    void startProcessing();
    void finishProcessing();
  public slots:
    void loadFromMemory();
    void saveSelection();
    void queueFetch();
  private slots:
};

#endif // ASSETTREE_H
