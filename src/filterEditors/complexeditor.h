#ifndef COMPLEXEDITOR_H
#define COMPLEXEDITOR_H

#include <QAbstractItemModel>
#include <QComboBox>

#include "bpfiltereditor.h"
//#include "../blueprintsfilterboxwidget.h"

class impComplexEditor;

class ComplexEditor : public BpFilterEditor
{
    Q_OBJECT
  public:
    ComplexEditor(QWidget* parent = nullptr);
    ComplexEditor(QSharedPointer<BpFilter> filter, QWidget* parent);
    ~ComplexEditor();

    void setFilter(const QSharedPointer<BpFilter>& filter) override;

  protected:
    void setEditorValue(const QVariant& val) override;

  private:
    QComboBox* typeBox;
    impComplexEditor* _imp; //remove circular dependencies

  private slots:
    void onFilterTypeChange();

};

#endif // COMPLEXEDITOR_H
