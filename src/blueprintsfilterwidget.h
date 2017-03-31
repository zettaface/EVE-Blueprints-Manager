#ifndef BLUEPRINTSFILTERWIDGET_H
#define BLUEPRINTSFILTERWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QDebug>
#include <QComboBox>
#include <QFrame>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QIntValidator>

#include "eve/staticdata.h"
#include "categoryfilterproxymodel.h"
#include "bpfilterfactory.h"

class BlueprintsFilterWidget : public QFrame
{
    Q_OBJECT
  public:
    explicit BlueprintsFilterWidget(QWidget* parent = 0);
    explicit BlueprintsFilterWidget(QSharedPointer<BpFilter> filter, QWidget* parent = 0);
    explicit BlueprintsFilterWidget(const QVector<int>& filterTypes, QSharedPointer<BpFilter> filter, QWidget* parent = 0);

    QSharedPointer<BpFilter> currentFilter() const;
    void setFilter(QSharedPointer<BpFilter> filter);

  signals:
    void filterTypeChanged(QSharedPointer<BpFilter>, QSharedPointer<BpFilter>);
    void typeChanged(int);

  public slots:
    void setType(int);

  private slots:
    void setEditorDefaults();
    void onFilterTypeBoxActivated(int);

  private:
    QWidget* getEditor(int ftype);

    QComboBox* filterTypeBox;
    QStackedWidget* filters;

    QSharedPointer<BpFilter> currentFilter_ ;
    QMap<int, QWidget*> editors_;
    BpFilterFactory factory_;
    const QVector<int> filterTypes_;

    enum Type
    {
      None = 0,
      CategoryGroup,
      MarketGroup,
      Name,
      Runs,
      Quantity,
      ME,
      TE,
      Logical,
      TypeCount
    };
};


#endif // BLUEPRINTSFILTERWIDGET_H
