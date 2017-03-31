#include "bpcolorsview.h"
#include <QDrag>

void BPColorsView::startDrag(Qt::DropActions supportedActions)
{
  QTableView::startDrag(supportedActions);
}

QModelIndexList BPColorsView::selectedDraggableIndexes() const
{
  QModelIndexList indexes = selectedIndexes();
  for(int i = indexes.count() - 1 ; i >= 0; --i) {
    if (!(model()->flags(indexes.at(i)) & Qt::ItemIsDragEnabled))
      indexes.removeAt(i);
  }
  return indexes;
}
