#ifndef BPCOLORDIALOG_H
#define BPCOLORDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QEvent>

#include <memory>

class BPListColorModel;

class BPColorDialog : public QDialog
{
    Q_OBJECT
  public:
    BPColorDialog(BPListColorModel* bpColors = nullptr, QWidget* parent = 0);
  private:
    QWidget* makeModelPage(BPListColorModel*);
    void spanMarketGroupRows(QTableView*);
    QTableView* view;

    BPListColorModel* _bpColors;
  private slots:
    void showContextMenu(QPoint pos, QTableView* view);
};

#endif // BPCOLORDIALOG_H
