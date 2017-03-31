#ifndef BPCOLORLISTITEMDELEGATE_H
#define BPCOLORLISTITEMDELEGATE_H

#include <QStyledItemDelegate>

class BPColorListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
  public:
    explicit BPColorListItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
    ~BPColorListItemDelegate() = default;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
  private:
    QWidget* createFilterEditor(QWidget* parent, const QModelIndex& index) const;
    QWidget* createPriorityEditor(QWidget* parent, const QModelIndex& index) const;

    void setFilterEditorData(const QModelIndex& index, QWidget* editor) const;
};

#endif // BPCOLORLISTITEMDELEGATE_H
