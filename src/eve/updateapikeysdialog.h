#ifndef UPDATEAPIKEYSDIALOG_H
#define UPDATEAPIKEYSDIALOG_H

#include <QDialog>
#include <QProgressBar>

#include "parser/apikeyupdater.h"

class UpdateApiKeysDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit UpdateApiKeysDialog(QWidget* parent = nullptr);
    QVector<eve::ApiKeyUpdater::Error> expiredKeys() const { return updater_->expiredKeys(); }

  public slots:
    int exec() override;

  protected:
    void keyPressEvent(QKeyEvent* event) override;

  private:
    QSharedPointer<eve::ApiKeyUpdater> updater_;
    QProgressBar* progress_;
    int parsedCount_ {0};

  private slots:
    void onKeyParseComplete();
};

#endif // UPDATEAPIKEYSDIALOG_H
