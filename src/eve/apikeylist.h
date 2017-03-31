#ifndef EVEAPIKEYLIST_H
#define EVEAPIKEYLIST_H

#include <QAbstractItemModel>
#include <QVector>

#include "apikey.h"

namespace eve {

class ApiKeyList : public QAbstractListModel
{
    Q_OBJECT
    using Container = QVector<ApiKeyInfo*>;
  public:
    using iterator = Container::iterator;
    using const_iterator = Container::const_iterator;

    ApiKeyList(QObject* parent = 0);

    const_iterator begin() const { return keys.begin(); }
    const_iterator end() const { return keys.end(); }
    iterator begin() { return keys.begin(); }
    iterator end() { return keys.end(); }

    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool removeRows(int row, int count, const QModelIndex& parent) override;

    void addKey(ApiKeyInfo* key);
    void removeKey(const ApiKeyInfo* key);

    int size() const { return keys.size(); }

    ApiKeyInfo* key(int i) const;
    ApiKeyInfo* keyByKeyID(int id) const;

    bool contains(ApiKeyInfo* key) const;
    bool isEmpty() const;

    Container getKeys() const;
    void setKeys(const Container& value);

    QDateTime nextAssetsFetchTime() const { return nextAssetsFetchTime_; }
    QDateTime nextBlueprintsFetchTime() const { return nextBlueprintsFetchTime_; }
    QDateTime nextIndustryJobsFetchTime() const { return nextIndustryJobsFetchTime_; }

    enum Columns {
      Name = 0,
      Type,
      KeyID,
      CharID,
      vCode,
      ColumnCount
    };
  public slots:
    void fetchFromDB();
  signals:
    void keysChanged();

  private:
    void updateTime(QDateTime& oldTime, const QDateTime& newTime);
    Container keys;

    QDateTime nextAssetsFetchTime_;
    QDateTime nextBlueprintsFetchTime_;
    QDateTime nextIndustryJobsFetchTime_;
};

} // namespace EVE

#endif // EVEAPIKEYLIST_H
