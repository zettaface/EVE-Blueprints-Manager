#ifndef COMBOLINEEDITOR_H
#define COMBOLINEEDITOR_H

#include <QComboBox>
#include <QIntValidator>
#include <QLineEdit>

#include "bpfiltereditor.h"

class ComboLineEditor : public BpFilterEditor
{
    Q_OBJECT
  public:
    ComboLineEditor(int func, QString val, QWidget* parent = nullptr);

    void setRange(int min, int max);
    void setDefaultValue() override;

  protected:
    void setEditorValue(const QVariant& val) override;

  private:
    void updateProxy(int val, int func);
    QComboBox* funcComboBox(QWidget* parent = nullptr) const;

    QIntValidator* validator;
    QLineEdit* valEdit;
    QComboBox* funcBox;

    QString defaultValue_;
    int     defaultFunc_;

  private slots:
    void updateValue(QString str);
    void updateFunctor(int);
    // BpFilterEditor interface
};

#endif // COMBOLINEEDITOR_H
