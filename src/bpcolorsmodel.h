#ifndef BPCOLORSMODEL_H
#define BPCOLORSMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QColor>
#include <QStyledItemDelegate>
#include <QMap>

#include "blueprintcolorizer.h"
#include "blueprintlist.h"


class BPListColorModel : public QAbstractTableModel
{
    Q_OBJECT
  public:
    BPListColorModel(QObject* parent = 0);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    Qt::DropActions supportedDropActions() const override;
    Qt::DropActions supportedDragActions() const override;
    QMap<int, QVariant> itemData(const QModelIndex& index) const override;
    bool setItemData(const QModelIndex& index, const QMap<int, QVariant>& roles) override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList& indexes) const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
    BlueprintColorizer findColorizer(const QModelIndex& index) const;

    void save(QIODevice* device);
    bool load(QIODevice* device);

    int count() const;

    enum Columns
    {
      Filter,
      ForegroundColor,
      BackgroundColor,
      Priority,
      ColumnCount
    };

  public slots:
    void fetchFromDB();
    void fetchFromDB(QString dbname);
    void submitToDB() const;
    void submitToDB(QString dbname) const;

  signals:
    void submitted();

  private:
    QMap<int, QString> funcNames_;
    QMap<int, QString> valNames_;
    QVector<BlueprintColorizer> data_;

    QString lastUsedDB;
};

#endif // BPCOLORSMODEL_H
