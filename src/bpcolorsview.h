#ifndef BPCOLORSVIEW_H
#define BPCOLORSVIEW_H

#include <QTableView>

class BPColorsView : public QTableView
{
    Q_OBJECT
  public:
    using QTableView::QTableView;

  protected:
    void startDrag(Qt::DropActions supportedActions) override;
  private:
    QModelIndexList selectedDraggableIndexes() const;
};

#endif // BPCOLORSVIEW_H
