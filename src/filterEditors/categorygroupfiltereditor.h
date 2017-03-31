#ifndef CATEGORYGROUPFILTEREDITOR_H
#define CATEGORYGROUPFILTEREDITOR_H

#include "bpfiltereditor.h"

#include <QComboBox>
#include "../categoryfilterproxymodel.h"

class CategoryGroupFilterEditor : public BpFilterEditor
{
    Q_OBJECT
  public:
    CategoryGroupFilterEditor(QWidget* parent = nullptr);
    CategoryGroupFilterEditor(QSharedPointer<BpFilter> filter, QWidget* parent = nullptr);

    void setDefaultValue() override;
  protected:
    void setEditorValue(const QVariant& val) override;

    int findCategoryRow(int ID);
    int findGroupRow(int ID);
  private:
    void updateProxy(int cid, int gid);

    CategoryFilterProxyModel* groupBoxFilter;
    QComboBox* categoryBox;
    QComboBox* groupBox;
  private slots:
    void updateCategory(int);
    void updateGroup(int);

};
#endif // CATEGORYGROUPFILTEREDITOR_H
