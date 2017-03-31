#include "oreselectionwidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDebug>

OreSelectionWidget::OreSelectionWidget(QWidget* parent) : QWidget(parent)
{
  l = new QGridLayout(this);
  l->addWidget(new QLabel("0%", this), 1, 0, 1, 1, Qt::AlignLeft);
  l->addWidget(new QLabel("5%", this), 2, 0, 1, 1, Qt::AlignLeft);
  l->addWidget(new QLabel("10%", this), 3, 0, 1, 1, Qt::AlignLeft);
}

QVector<ItemRefinery> OreSelectionWidget::selectedItems() const
{
  return model_->selectedItems();
}

OreTableModel* OreSelectionWidget::model() const
{
  return model_;
}

void OreSelectionWidget::setModel(OreTableModel* model)
{
  model_ = model;
  connect(model_, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this, SLOT(onModelDataChanged(QModelIndex, QModelIndex, QVector<int>)));

  for(QCheckBox* c : checkboxes_) {
    l->removeWidget(c);
    c->deleteLater();
  }

  checkboxes_.clear();


  for (int i = 0; i < model_->rowCount() / 3; i++) {
    QLabel* icon = new QLabel("x", this);
    icon->setToolTip(model_->data(model_->index(i * 3, 0), Qt::DisplayRole).toString());
    icon->setPixmap(model_->data(model_->index(i * 3, 0), Qt::DecorationRole).value<QPixmap>());
    l->addWidget(icon, 0, i + 1, 1, 1, Qt::AlignCenter);

    for (int j = 0; j < 3; j++) {
      QCheckBox* checkbox = new QCheckBox(this);
      QModelIndex index = model_->index(i * 3 + j, 0);
      checkbox->setToolTip(index.data().toString());
      checkbox->setCheckState(index.data(Qt::CheckStateRole).value<Qt::CheckState>());
      connect(checkbox, &QCheckBox::clicked, [this, i, j](bool checked) {
        Qt::CheckState checkstate = checked ? Qt::Checked : Qt::Unchecked;
        qDebug() << checkstate;
        model_->setData(model_->index(i * 3 + j, 0), checkstate, Qt::CheckStateRole);
      });

      checkboxes_.push_back(checkbox);
      l->addWidget(checkbox, j + 1, i + 1, 1, 1, Qt::AlignCenter);
    }
  }
}

void OreSelectionWidget::onModelDataChanged(const QModelIndex& tl, const QModelIndex& br, const QVector<int>& roles)
{
  checkboxes_[tl.row()]->setCheckState(tl.data(Qt::CheckStateRole).value<Qt::CheckState>());
}
