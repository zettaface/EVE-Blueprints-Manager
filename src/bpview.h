#ifndef BPVIEW_H
#define BPVIEW_H

#include <QTreeView>

class BpView : public QTreeView
{
  public:
    using QTreeView::QTreeView;

  protected:
    void keyPressEvent(QKeyEvent*) override;

  private:
    void copy();
};



#endif // BPVIEW_H
