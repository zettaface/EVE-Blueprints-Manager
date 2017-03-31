#include "apikeymanagmentdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAction>
#include <QMenu>

#include "eve/api.h"
#include "apikeyinsertdialog.h"

ApiKeyManagmentDialog::ApiKeyManagmentDialog(QWidget* parent) :
  QDialog(parent)
{
  view = new QTreeView(this);
  addKeysBtn = new QPushButton("Add Keys", this);
  connect(addKeysBtn, &QPushButton::clicked, [this]() {
    ApiKeyInsertDialog dial(this);
    dial.exec();
  });

  view->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
  //view->addAction(removeKey);

  closeBtn = new QPushButton("Close", this);
  connect(closeBtn, SIGNAL(clicked()), this, SLOT(accept()));

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  buttonLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
  buttonLayout->addWidget(addKeysBtn);
  buttonLayout->addWidget(closeBtn);

  QVBoxLayout* container = new QVBoxLayout(this);
  container->addWidget(view);
  container->addLayout(buttonLayout);

  QAbstractListModel* model = eve::API.keys();
  view->setModel(model);

  setMinimumSize(600, 150);
}

void ApiKeyManagmentDialog::showContextMenu(QPoint pos)
{
  QModelIndex index = view->indexAt(pos);
  if (!index.isValid())
    return;

  QAction* removeKey = new QAction("Remove character", this);
  connect(removeKey, &QAction::triggered, [this, pos]() {
    QModelIndexList selection = view->selectionModel()->selectedIndexes();
    eve::ApiKeyList* model = static_cast<eve::ApiKeyList*>(view->model()); //FIXME
    model->removeRow(view->indexAt(pos).row());
  });

  QMenu menu;// = new QMenu(view);
  menu.addAction(removeKey);
  menu.exec(QCursor::pos());
}
