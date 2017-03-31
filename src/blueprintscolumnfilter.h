#ifndef BLUEPRINTSCOLUMNFILTER_H
#define BLUEPRINTSCOLUMNFILTER_H

#include <QSortFilterProxyModel>
#include <QSet>

class BlueprintsColumnFilter : public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    explicit BlueprintsColumnFilter(QObject* parent = nullptr);
    BlueprintsColumnFilter(const QSet<int>& filteredColumns, QObject* parent = nullptr);

    QSet<int> filteredColumns() const;
    void setFilteredColumns(const QSet<int>& filteredColumns);

  public slots:
    void filterColumn(int column, bool filter);
  private:
    QSet<int> filteredColumns_;

  protected:
    bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;
};

#endif // BLUEPRINTSCOLUMNFILTER_H
