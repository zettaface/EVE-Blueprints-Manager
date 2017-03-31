#include "updateapikeysdialog.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QKeyEvent>

#include "api.h"

UpdateApiKeysDialog::UpdateApiKeysDialog(QWidget* parent) :
  QDialog(parent, Qt::WindowTitleHint)
{
  setWindowTitle("Checking keys");
  QLabel* label = new QLabel("Checking API keys", this);
  progress_ = new QProgressBar(this);
  progress_->setRange(0, eve::API.keys()->size());

  QVBoxLayout* l = new QVBoxLayout(this);
  l->addWidget(label);
  l->addWidget(progress_);

  updater_ = QSharedPointer<eve::ApiKeyUpdater>::create();

  connect(updater_.data(), &eve::ApiKeyUpdater::parsed, this, &UpdateApiKeysDialog::onKeyParseComplete);
  eve::API.updateApiKeys(updater_);
}

void UpdateApiKeysDialog::keyPressEvent(QKeyEvent* event)
{
  if (event->key() != Qt::Key_Escape)
    QDialog::keyPressEvent(event);
}

void UpdateApiKeysDialog::onKeyParseComplete()
{
  parsedCount_++;
  progress_->setValue(parsedCount_);

  if (parsedCount_ == eve::API.keys()->size())
    accept();
}
