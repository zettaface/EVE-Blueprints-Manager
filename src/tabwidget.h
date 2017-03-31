#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QPoint>

class TabWidget : public QTabWidget
{
    Q_OBJECT
  public:
    TabWidget(QWidget* parent = nullptr) : QTabWidget(parent) {}
  protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
  private:
    QPoint initialClickPos;
    int    ripOffDist;
};

#endif // TABWIDGET_H
