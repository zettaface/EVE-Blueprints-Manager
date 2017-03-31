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

  protected:
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

  private:
    int type_;

    bool colorLess(const QModelIndex& source_left, const QModelIndex& source_right) const;
    bool numericLess(int column, const QModelIndex& source_left, const QModelIndex& source_right) const;
    bool stringLess(int column, const QModelIndex& source_left, const QModelIndex& source_right) const;
};

#endif // BLUEPRINTSSORTPROXY_H
