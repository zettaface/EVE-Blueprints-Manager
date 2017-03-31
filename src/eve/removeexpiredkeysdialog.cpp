#include "removeexpiredkeysdialog.h"

#include "api.h"

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>

RemoveExpiredKeysDialog::RemoveExpiredKeysDialog(const QVector<Error>& errors, QWidget* parent) :
  QDialog(parent, Qt::WindowTitleHint)
{
  QLabel* messageLabel = new QLabel("An errors occured while queryng API keys", this);
  view_ = new QListView(this);

  removeButton_ = new QPushButton("Remove selected keys", this);
  cancelButton_ = new QPushButton("Cancel", this);

  model_ = new eve::KeyStatusParserErrorModel(errors, this);
  view_->setModel(model_);

  QHBoxLayout* buttonsLayout = new QHBoxLayout;
  buttonsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
  buttonsLayout->addWidget(removeButton_);
  buttonsLayout->addWidget(cancelButton_);

  QVBoxLayout* l = new QVBoxLayout(this);
  l->addWidget(messageLabel);
  l->addWidget(view_, 1);
  l->addLayout(buttonsLayout);

  connect(removeButton_, SIGNAL(clicked(bool)), this, SLOT(removeSelectedKeys()));
  connect(cancelButton_, SIGNAL(clicked(bool)), this, SLOT(reject()));
}

void RemoveExpiredKeysDialog::removeSelectedKeys()
{
  using Role = eve::KeyStatusParserErrorModel::Role;

  for (int i = 0; i < model_->rowCount(QModelIndex()); i++) {
    QModelIndex index = model_->index(i, 0);

    if (index.data(Qt::CheckStateRole).value<Qt::CheckState>() == Qt::Checked) {
      int keyID = index.data(Role::ID).toInt();

      eve::API.removeKeyById(keyID);
    }
  }
  qDebug() << "Keys deleted";
  accept();
}
