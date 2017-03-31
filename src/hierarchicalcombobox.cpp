#include "hierarchicalcombobox.h"

#include <QHeaderView>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>

HierarchicalComboBox::HierarchicalComboBox(QWidget* parent) :
  QComboBox(parent)
{
  _treeView = new QTreeView(this);
  _treeView->setFrameShape(QFrame::NoFrame);
  _treeView->setEditTriggers(QTreeView::NoEditTriggers);
  _treeView->setAlternatingRowColors(true);
  _treeView->setSelectionBehavior(QTreeView::SelectRows);
  _treeView->setRootIsDecorated(true);
  _treeView->setWordWrap(true);
  _treeView->setAllColumnsShowFocus(true);
  _treeView->setItemsExpandable(true);
  _treeView->header()->setVisible(false);

  setView(_treeView);
  view()->viewport()->installEventFilter(this);

}

void HierarchicalComboBox::showPopup()
{
  setRootModelIndex(QModelIndex());
  QComboBox::showPopup();

  for(int i = 1; i < model()->columnCount(); ++i)
    _treeView->hideColumn(i);

  if (_treeView->selectionModel()->selectedIndexes().isEmpty())
    return;

  qDebug() << "Selection size is " << _treeView->selectionModel()->selectedIndexes().size();
  QModelIndex selection = _treeView->selectionModel()->selectedIndexes().first();

  if (!selection.isValid())
    return;
}

void HierarchicalComboBox::selectIndex(const QModelIndex& index)
{
  setRootModelIndex(index.parent());
  setCurrentIndex(index.row());
  setRootModelIndex(QModelIndex());
  _treeView->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
}

void HierarchicalComboBox::hidePopup()
{
  if (skipNextHide)
    skipNextHide = false;
  else
    QComboBox::hidePopup();
}

bool HierarchicalComboBox::eventFilter(QObject* object, QEvent* event)
{
  if (event->type() == QEvent::MouseButtonPress && object == view()->viewport()) {
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    QModelIndex index = view()->indexAt(mouseEvent->pos());
    if (!view()->visualRect(index).contains(mouseEvent->pos()))
      skipNextHide = true;
  }

  return false;
}
