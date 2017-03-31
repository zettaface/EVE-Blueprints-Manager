#ifndef ORESELECTIONWIDGET_H
#define ORESELECTIONWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QGridLayout>
#include <QVector>

#include "oretablemodel.h"

class OreSelectionWidget : public QWidget
{
    Q_OBJECT
  public:
    OreSelectionWidget(QWidget* parent = 0);

    QVector<ItemRefinery> selectedItems() const;
    OreTableModel* model() const;

    void setModel(OreTableModel* model);

  private slots:
    void onModelDataChanged(const QModelIndex& tl, const QModelIndex& br, const QVector<int>& roles);

  private:
    QVector<QCheckBox*> checkboxes_;
    OreTableModel* model_;
    QGridLayout* l;
};

#endif // ORESELECTIONWIDGET_H
