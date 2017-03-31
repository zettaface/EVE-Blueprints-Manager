#ifndef BLUEPRINTSEMPTYGROUPSFILTER_H
#define BLUEPRINTSEMPTYGROUPSFILTER_H

#include <QSortFilterProxyModel>


class BlueprintsEmptyGroupsFilter : public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    explicit BlueprintsEmptyGroupsFilter(QObject* parent = nullptr) : QSortFilterProxyModel(parent) {}

  public slots:
    void setEnabled(bool);
    void setDisabled(bool);

  private:
    bool enabled_ {true};

  protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
};

#endif // BLUEPRINTSEMPTYGROUPSFILTER_H
