#ifndef SINGLELINEEDITOR_H
#define SINGLELINEEDITOR_H

#include <QLineEdit>

#include "bpfiltereditor.h"

class SingleLineEditor : public BpFilterEditor
{
    Q_OBJECT
  public:
    SingleLineEditor(QWidget* parent = nullptr);
    SingleLineEditor(QSharedPointer<BpFilter> filter, QWidget* parent = nullptr);
    void setDefaultValue() override;

  protected:
    void setEditorValue(const QVariant& val) override;

  private:
    QLineEdit* editor;
    QString defaultValue_;

  private slots:
    void updateValue(QString str);
};


#endif // SINGLELINEEDITOR_H
