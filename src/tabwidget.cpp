#include "tabwidget.h"

#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QVBoxLayout>
#include <QDebug>

void TabWidget::mousePressEvent(QMouseEvent* event)
{
  return QTabWidget::mousePressEvent(event);

  initialClickPos = event->pos();
  qDebug() << initialClickPos;
}

void TabWidget::mouseReleaseEvent(QMouseEvent* event)
{
  return QTabWidget::mouseReleaseEvent(event);

  initialClickPos = QPoint();
}

void TabWidget::dragMoveEvent(QDragMoveEvent* event)
{
  return QTabWidget::dragMoveEvent(event);

  if (!initialClickPos.isNull() && std::abs(event->pos().y() - initialClickPos.y()) > ripOffDist) {
    qDebug() << "Ripped!";
    QWidget* cont = new QWidget;
    QWidget* ripOffedTab = currentWidget();
    removeTab(currentIndex());

    QVBoxLayout* l = new QVBoxLayout(cont);
    l->addWidget(ripOffedTab);

    cont->show();
  }

}
