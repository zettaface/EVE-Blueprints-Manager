#ifndef BPFILTEREDITOR_H
#define BPFILTEREDITOR_H

#include <QWidget>
#include <QDebug>
#include <QSharedPointer>

#include "../filters/bpfilter.h"

class BpFilterEditor : public QWidget
{
    Q_OBJECT
  public:
    BpFilterEditor(QWidget* parent = nullptr);
    BpFilterEditor(QSharedPointer<BpFilter> filter, QWidget* parent = nullptr);

    virtual ~BpFilterEditor() {}

    virtual void setDefaultValue() {}
    virtual QSharedPointer<BpFilter> filter() const;
    virtual void setFilter(const QSharedPointer<BpFilter>& filter);
  protected:
    void showEvent(QShowEvent* e) override;
    void initialize();
  private:
    bool initialized_ {false};
  protected:
    virtual void setEditorValue(const QVariant& val) {}

    QSharedPointer<BpFilter> filter_;
};




#endif // BPFILTEREDITOR_H
