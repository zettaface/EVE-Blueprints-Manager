#ifndef INDUSTRYACTIVITYTYPEMODEL_H
#define INDUSTRYACTIVITYTYPEMODEL_H

#include <QAbstractListModel>

struct IndustryActivity
{
  QString name;
  QString desc;
  int id;
};

class IndustryActivityTypeModel : public QAbstractListModel
{
    Q_OBJECT
  public:
    explicit IndustryActivityTypeModel(QObject* parent = 0) : QAbstractListModel(parent) {}

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    void fetchFromSDE();
    QString activityName(int activityID) const;

  private:
    QVector<IndustryActivity> data_;
};

#endif // INDUSTRYACTIVITYTYPEMODEL_H
