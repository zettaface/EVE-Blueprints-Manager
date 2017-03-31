#include "removeduplicatekeysdialog.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QGroupBox>
#include <QPushButton>
#include <QDebug>
#include <QtSql>

#include "api.h"

RemoveDuplicateKeysDialog::RemoveDuplicateKeysDialog(QWidget* parent) :
  QDialog(parent, Qt::WindowTitleHint)
{
  setWindowTitle("Duplicate keys found");
  duplicates_ = findDuplicates();

  QVBoxLayout* l = new QVBoxLayout(this);
  QLabel* label = new QLabel("Some duplicate keys was detected", this);

  l->addWidget(label);

  for (auto& bin : duplicates_) {
    QString title;
    if (bin[0]->type() == "char")
      title = QString("Character: %1").arg(bin[0]->charName());
    else
      title = QString("Corporation: %1").arg(bin[0]->corpName());

    QGroupBox* keyGroup = new QGroupBox(this);
    keyGroup->setTitle(title);
    keyGroup->setCheckable(true);
    keyGroup->setChecked(false);

    connect(keyGroup, &QGroupBox::toggled, [this, bin](bool checked) {
      if (!checked) {
        for (auto key : bin)
          removeList_.remove(key->ID());
      }
      qDebug() << removeList_;
    });

    QVBoxLayout* groupLayout = new QVBoxLayout(keyGroup);

    for (auto key : bin) {
      QString title = QString("key: %1, vCode: %2, access mask %3").arg(key->keyID(), key->vCode()).arg(key->accessMask());
      QRadioButton* radio = new QRadioButton(title, this);
      groupLayout->addWidget(radio);

      connect(radio, &QRadioButton::toggled, [this, key](bool checked) {
        if (checked)
          removeList_.remove(key->ID());
        else
          removeList_.insert(key->ID());

        qDebug() << removeList_ << key->charName();
      });

      connect(keyGroup, &QGroupBox::toggled, [this, key, radio](bool checked) {
        if (checked && !radio->isChecked())
          removeList_.insert(key->ID());

        qDebug() << removeList_ << key->charName();
      });

    }

    keyGroup->setLayout(groupLayout);
    l->addWidget(keyGroup);
  }

  QPushButton* removeButton = new QPushButton("Keep selected");
  QPushButton* cancelButton = new QPushButton("Cancel");

  connect(removeButton, &QPushButton::clicked, this, &RemoveDuplicateKeysDialog::removeDuplicates);
  connect(cancelButton, &QPushButton::clicked, this, &RemoveDuplicateKeysDialog::reject);

  QHBoxLayout* buttonsLayout = new QHBoxLayout;
  buttonsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
  buttonsLayout->addWidget(removeButton);
  buttonsLayout->addWidget(cancelButton);

  l->addLayout(buttonsLayout);
}

QVector<QVector<eve::ApiKeyInfo*>> RemoveDuplicateKeysDialog::findDuplicates() const
{
  using MapKey = QPair<QString, QString>;

  auto keys = eve::API.keys();
  QMap<MapKey, QVector<eve::ApiKeyInfo*>> aggregation;
  QVector<QVector<eve::ApiKeyInfo*>> duplicates;

  for (eve::ApiKeyInfo* key : *keys) {
    QString keyType = key->type();
    if (keyType == "corp") {
      MapKey p { keyType, key->corpName() };
      aggregation[p].push_back(key);
    } else if (keyType == "char") {
      MapKey p { keyType, key->charName() };
      aggregation[p].push_back(key);
    }
  }

  auto bins = aggregation.keys();
  for (auto bin : bins) {
    if (aggregation[bin].size() > 1)
      duplicates.push_back(aggregation[bin]);
  }

  return duplicates;
}

void RemoveDuplicateKeysDialog::removeDuplicates()
{
  for (auto keyID : removeList_)
    eve::API.removeKeyById(keyID);

  accept();
}
