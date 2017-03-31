#ifndef ISHEADERFILTEREDITOR_H
#define ISHEADERFILTEREDITOR_H

#include "bpfiltereditor.h"
#include <QComboBox>

class IsHeaderFilterEditor : public BpFilterEditor
{
    Q_OBJECT
  public:
    IsHeaderFilterEditor(QWidget* parent = nullptr);

  protected:
    void setEditorValue(const QVariant& val) override;

  private:
    QComboBox* editor;

  private slots:
    void onTypeChanged(int);

};



#endif // ISHEADERFILTEREDITOR_H
