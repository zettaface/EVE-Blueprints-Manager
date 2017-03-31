#ifndef BLUEPRINTPAGE_H
#define BLUEPRINTPAGE_H

#include <QWidget>
#include <QTreeView>
#include <QVariantAnimation>
#include <QLineEdit>

#include "blueprint.h"
#include "productionmodel.h"

class BlueprintPage : public QWidget
{
    Q_OBJECT
  public:
    explicit BlueprintPage(const Blueprint& bp, QWidget* parent = 0);

  private:
    double currentPrice_ {0};

    QLineEdit* buildPriceEdit;
    QLineEdit* marketPriceEdit;

    QTreeView* componentsView_;
    ProductionModel* model_;
    QVariantAnimation* buildPriceAnimation_;

  private slots:
    void onPriceChange(double);
    void updatePriceBox(QVariant);
};

#endif // BLUEPRINTPAGE_H
