#ifndef BLUEPRINTSSORTPROXY_H
#define BLUEPRINTSSORTPROXY_H

#include <QSortFilterProxyModel>

class BlueprintsSortProxy : public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    BlueprintsSortProxy(QObject* parent = nullptr);

    int type() const;
    void setType(int type);
    void sort(int column);

    enum Type
    {
      Name = 0,
      Runs,
      Quantity,
      Location,
      ME,
      TE,
      Color
    };
  public slots:
    void setSortOrder(Qt::SortOrder order) { order_ = order; }

  protected:
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

  private:
    int type_ { Name };
    Qt::SortOrder order_ { Qt::AscendingOrder };

    bool colorLess(const QModelIndex& source_left, const QModelIndex& source_right) const;
    bool numericLess(int column, const QModelIndex& source_left, const QModelIndex& source_right) const;
    bool stringLess(int column, const QModelIndex& source_left, const QModelIndex& source_right) const;
};

#endif // BLUEPRINTSSORTPROXY_H
