#ifndef MARKETGROUPFILTEREDITOR_H
#define MARKETGROUPFILTEREDITOR_H

#include "bpfiltereditor.h"
#include "../hierarchicalcombobox.h"

class MarketGroupFilterEditor : public BpFilterEditor
{
    Q_OBJECT
  public:
    MarketGroupFilterEditor(QWidget* parent = nullptr);
    MarketGroupFilterEditor(QSharedPointer<BpFilter> filter, QWidget* parent = nullptr);
    void setDefaultValue() override;

  protected:
    void setEditorValue(const QVariant& val) override;

  private:
    HierarchicalComboBox* marketIDFilterBox;

  private slots:
    void updateMarketGroup(int);
};



#endif // MARKETGROUPFILTEREDITOR_H
