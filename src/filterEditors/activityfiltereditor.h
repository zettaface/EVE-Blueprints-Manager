#ifndef ACTIVITYFILTEREDITOR_H
#define ACTIVITYFILTEREDITOR_H

#include "bpfiltereditor.h"
#include <QComboBox>

class ActivityFilterEditor : public BpFilterEditor
{
    Q_OBJECT
  public:
    ActivityFilterEditor(QWidget* parent = nullptr);
    ActivityFilterEditor(QSharedPointer<BpFilter> filter, QWidget* parent = nullptr);

  protected:
    void setEditorValue(const QVariant& val) override;

  private:
    QComboBox* activityEditor;

  private slots:
    void onActivityTypeChanged(int);
};

#endif // ACTIVITYFILTEREDITOR_H
