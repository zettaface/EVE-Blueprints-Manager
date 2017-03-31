#include "bpcolordialog.h"

#include <QTableView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QDebug>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QAction>
#include <QMenu>
#include <QEvent>
#include <QTabWidget>
#include <QFileDialog>

#include "bpcolorsmodel.h"
#include "bpfilterfactory.h"
#include "bpcolorlistitemdelegate.h"

BPColorDialog::BPColorDialog(BPListColorModel* bpColors, QWidget* parent) :
  QDialog(parent), _bpColors {bpColors}
{
  QWidget* bpColorsPage = makeModelPage(_bpColors);

  QDialogButtonBox* buttons = new QDialogButtonBox(this);
  buttons->addButton(QDialogButtonBox::StandardButton::Ok);
  buttons->addButton(QDialogButtonBox::StandardButton::Cancel);
  buttons->addButton(QDialogButtonBox::StandardButton::Apply);
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout* l = new QVBoxLayout(this);
  l->addWidget(bpColorsPage);
  l->addWidget(buttons);

  connect(this, SIGNAL(accepted()), _bpColors, SLOT(submitToDB()));
  connect(this, SIGNAL(rejected()), _bpColors, SLOT(fetchFromDB()));

  QPushButton* applyButton = buttons->button(QDialogButtonBox::StandardButton::Apply);
  connect(applyButton, SIGNAL(clicked()), _bpColors, SLOT(submitToDB()));
}

QWidget* BPColorDialog::makeModelPage(BPListColorModel* model)
{
  QTableView* view = new QTableView(this);
  QPushButton* addColor = new QPushButton("+", this);
  QPushButton* removeColor = new QPushButton("-", this);
  QPushButton* exportModel = new QPushButton("Export...", this);
  QPushButton* importModel = new QPushButton("Import...", this);

  removeColor->setMinimumWidth(removeColor->minimumHeight());

  QHBoxLayout* buttonsLayout = new QHBoxLayout;
  buttonsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
  buttonsLayout->addWidget(exportModel);
  buttonsLayout->addWidget(importModel);
  buttonsLayout->addWidget(addColor);
  buttonsLayout->addWidget(removeColor);

  QVBoxLayout* container = new QVBoxLayout;
  container->addWidget(view);
  container->addLayout(buttonsLayout);

  view->setModel(model);
  view->setItemDelegate(new BPColorListItemDelegate);
  connect(view, &QTableView::doubleClicked, [view](const QModelIndex & index) {
    if (index.column() == BPListColorModel::Filter)
      view->resizeRowToContents(index.row());
    qDebug() << "Entered";
  });

  QHeaderView* headerView = new QHeaderView(Qt::Horizontal, this);
  view->setHorizontalHeader(headerView);
  view->verticalHeader()->setVisible(true);
  headerView->setMinimumSectionSize(40);
  headerView->setDragEnabled(true);
  headerView->setVisible(true);

  view->setColumnWidth(BPListColorModel::Filter, 400);
  view->setColumnWidth(BPListColorModel::BackgroundColor, 120);
  view->setColumnWidth(BPListColorModel::ForegroundColor, 100);

  view->setSelectionMode(QAbstractItemView::ExtendedSelection);
  view->setSelectionBehavior(QAbstractItemView::SelectRows);
  view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  view->resizeRowsToContents();
  view->setContextMenuPolicy(Qt::CustomContextMenu);
  view->setWordWrap(false);
  view->verticalHeader()->setDefaultSectionSize(view->verticalHeader()->minimumSectionSize());
  view->setTextElideMode(Qt::ElideRight);
  view->setAlternatingRowColors(true);

  view->setDragEnabled(true);
  view->setDropIndicatorShown(true);
  view->setAcceptDrops(true);
  view->setDefaultDropAction(Qt::MoveAction);
  view->setDragDropOverwriteMode(false);
  view->setDragDropMode(QAbstractItemView::DragDrop);
  view->installEventFilter(model);

  setMinimumWidth(500);
  setWindowTitle(tr("Color managment"));


  connect(view, &QTableView::doubleClicked, [this, model](const QModelIndex & index) {
    if (index.column() != BPListColorModel::BackgroundColor &&
        index.column() != BPListColorModel::ForegroundColor)
      return;

    QColor color = QColorDialog::getColor(model->data(index, Qt::DecorationRole).value<QColor>());
    model->setData(index, color, Qt::UserRole);
  });

  connect(addColor, &QPushButton::clicked, [model](bool) {
    model->insertRows(model->rowCount(), 1);
  });

  connect(exportModel, &QPushButton::clicked, [model, this](bool) {
    QString filename = QFileDialog::getSaveFileName(this, "Export", "", "YAML (*.yaml)");
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    model->save(&file);
  });

  connect(importModel, &QPushButton::clicked, [model](bool) {
    QString filename = QFileDialog::getOpenFileName(nullptr, "Import", "", "YAML (*.yaml)");
    if (filename.isNull())
      return;

    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    model->load(&file);
  });

  connect(view, &QTableView::customContextMenuRequested, [this, view](QPoint pos) {
    showContextMenu(pos, view);
  });

  QWidget* page = new QWidget;
  page->setLayout(container);
  return page;
}

void BPColorDialog::showContextMenu(QPoint pos, QTableView* view)
{
  QModelIndex index = view->indexAt(pos);
  int rowID = index.row();
  if (!index.isValid())
    return;

  int selectedRows = view->selectionModel()->selectedIndexes().size() / view->model()->columnCount();
  qDebug() << selectedRows;

  QString dupKeyString = QString("Duplicate color%1(%2)")
                         .arg(selectedRows > 1 ? "s" : "")
                         .arg(selectedRows);
  QAction* dupKey = new QAction(dupKeyString, this);
  connect(dupKey, &QAction::triggered, [this, view, selectedRows]() {
    QModelIndexList selection = view->selectionModel()->selectedIndexes();
    QAbstractItemModel* model = view->model();
    int firstRow = selection[0].row();
    int firstNewRow = firstRow + selectedRows;

    model->insertRows(firstNewRow, selectedRows);
    for (const QModelIndex& i : selection) {
      if (i.column() == BPListColorModel::Filter) {
        //NOTE temporary workaround

        using Ptr = QSharedPointer<BpFilter>;
        Ptr filter = i.data(Qt::UserRole).value<Ptr>();
        QByteArray data;
        QDataStream s(&data, QIODevice::ReadWrite);
        s << filter;
        s.device()->reset();
        Ptr filterCopy;
        s >> filterCopy;

        model->setData(model->index(i.row() + selectedRows, i.column()), QVariant::fromValue<Ptr>(filterCopy), Qt::UserRole);
      } else
        model->setData(model->index(i.row() + selectedRows, i.column()), i.data(Qt::UserRole), Qt::UserRole);
    }

  });

  QAction* removeKey = new QAction("Remove color", this);
  connect(removeKey, &QAction::triggered, [this, view, selectedRows]() {
    QModelIndexList selection = view->selectionModel()->selectedIndexes();
    QAbstractItemModel* model = view->model();
    int firstRow = selection[0].row();
    model->removeRows(firstRow, selectedRows);
  });

  QMenu menu;
  menu.addAction(dupKey);
  menu.addAction(removeKey);
  menu.exec(QCursor::pos());
}
