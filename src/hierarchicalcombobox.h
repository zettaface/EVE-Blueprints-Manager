#ifndef HIERARCHICALCOMBOBOX_H
#define HIERARCHICALCOMBOBOX_H

#include <QWidget>
#include <QComboBox>
#include <QTreeView>

class HierarchicalComboBox : public QComboBox
{
  public:
    HierarchicalComboBox(QWidget* parent = 0);

    void showPopup() override;
    void hidePopup() override;
    bool eventFilter(QObject*, QEvent*) override;

  private:
    QTreeView* _treeView;
    bool skipNextHide {false};

  public slots:
    void selectIndex(const QModelIndex&);
};

#endif // HIERARCHICALCOMBOBOX_H
