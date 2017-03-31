#include "bpview.h"

#include <QKeyEvent>
#include <QClipboard>
#include <QApplication>

void BpView::keyPressEvent(QKeyEvent* e)
{
  if (e->matches(QKeySequence::Copy))
    copy();
  else
    QTreeView::keyPressEvent(e);
}

void BpView::copy()
{
  QItemSelectionModel* selection = selectionModel();
  QModelIndexList indexes = selection->selectedIndexes();

  QString selected_text;
  QModelIndex previous = indexes.first();

  for(auto current : indexes) {
    if (current.row() != previous.row())
      selected_text.append('\n');
    else {
      if (previous != current)
        selected_text.append('\t');
    }
    QVariant data = model()->data(current);
    QString text = data.toString();
    selected_text.append(text);

    previous = current;
  }

  QApplication::clipboard()->setText(selected_text);
}
