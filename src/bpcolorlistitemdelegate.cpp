#include "bpcolorlistitemdelegate.h"

#include <QComboBox>
#include <QTableView>
#include <QLineEdit>
#include <QIntValidator>
#include <QEvent>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>

#include <QTextDocument>
#include <QPainter>
#include <QAbstractTextDocumentLayout>

#include "bpcolorsmodel.h"
#include "categoryfilterproxymodel.h"
#include "eve/api.h"
#include "eve/staticdata.h"
#include "hierarchicalcombobox.h"
#include "blueprintsfilterboxwidget.h"

class ScrollEventBlocker : public QObject
{
  public:
    bool eventFilter(QObject* object, QEvent* event) override {
      if (event->type() == QEvent::Wheel) {
        QComboBox* editor = static_cast<QComboBox*>(object);
        if (editor && !editor->hasFocus()) {
          event->ignore();
          return true;
        }
      }

      return false;
    }
};

QWidget* BPColorListItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  using Column = BPListColorModel::Columns;
  switch(index.column()) {
    case Column::Filter:
      return createFilterEditor(parent, index);
    case Column::Priority:
      return createPriorityEditor(parent, index);
    default:
      return QStyledItemDelegate::createEditor(parent, option, index);
  }
}

void BPColorListItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  using Column = BPListColorModel::Columns;
  switch(index.column()) {
    case Column::Filter:
      setFilterEditorData(index, editor);
      return;
    default:
      QStyledItemDelegate::setEditorData(editor, index);
      return;
  }
}


void BPColorListItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  using Column = BPListColorModel::Columns;
  switch(index.column()) {
    default:
      QStyledItemDelegate::setModelData(editor, model, index);
      return;
  }
}

QSize BPColorListItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if (index.column() != BPListColorModel::Filter)
    return QStyledItemDelegate::sizeHint(option, index);

  return QStyledItemDelegate::sizeHint(option, index);
}

class EditorDialog : public QDialog
{
  public:
    EditorDialog(QSharedPointer<BpComplexFilter> filter, QWidget* parent = nullptr, Qt::WindowFlags f = 0) :
      QDialog(parent, f) {
      editor = new BlueprintsFilterBoxWidget(filter);
      QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
      connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

      QFrame* line = new QFrame(this);
      line->setFrameShape(QFrame::HLine);
      line->setFrameShadow(QFrame::Sunken);

      QVBoxLayout* l = new QVBoxLayout(this);
      l->addWidget(editor);
      l->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding));
      l->addWidget(line);
      l->addWidget(buttonBox);
    }
  private:
    BlueprintsFilterBoxWidget* editor;
};

QWidget* BPColorListItemDelegate::createFilterEditor(QWidget* parent, const QModelIndex& index) const
{
  QPushButton* openEditorBtn = new QPushButton("Open in filter editor", parent);

  QSharedPointer<BpFilter> filter = index.data(Qt::UserRole).value<QSharedPointer<BpFilter>>();
  connect(openEditorBtn, &QPushButton::clicked, [this, filter, openEditorBtn](bool) {
    EditorDialog* dialog = new EditorDialog(qSharedPointerCast<BpComplexFilter>(filter));

    dialog->exec();
    dialog->deleteLater();
  });

  return openEditorBtn;
}

QWidget* BPColorListItemDelegate::createPriorityEditor(QWidget* parent, const QModelIndex& index) const
{
  QLineEdit* editor = new QLineEdit(parent);
  editor->setValidator(new QIntValidator(editor));

  return editor;
}

void BPColorListItemDelegate::setFilterEditorData(const QModelIndex& index, QWidget* editor) const
{
  return;
  BlueprintsFilterBoxWidget* b = static_cast<BlueprintsFilterBoxWidget*>(editor);
  QSharedPointer<BpFilter> filter = index.data(Qt::UserRole).value<QSharedPointer<BpFilter>>();
  b->setFilter(qSharedPointerCast<BpComplexFilter>(filter));
}

void BPColorListItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if (index.column() != BPListColorModel::Filter) {
    QStyledItemDelegate::paint(painter, option, index);
    return;
  }

  QStyleOptionViewItemV4 options = option;
  initStyleOption(&options, index);

  painter->save();

  QTextDocument doc;
  doc.setHtml(options.text);

  options.text = "";
  options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

  // shift text right to make icon visible
  QSize iconSize = options.icon.actualSize(options.rect.size());
  painter->translate(options.rect.left() + iconSize.width(), options.rect.top());
  QRect clip(0, 0, options.rect.width() + iconSize.width(), options.rect.height());

  painter->setClipRect(clip);
  QAbstractTextDocumentLayout::PaintContext ctx;
  // set text color to red for selected item
  ctx.palette.setColor(QPalette::Text, options.palette.color(QPalette::Text) /*QColor("red")*/);
  ctx.clip = clip;
  doc.documentLayout()->draw(painter, ctx);

  painter->restore();
}
