#ifndef REMOVEEXPIREDKEYSDIALOG_H
#define REMOVEEXPIREDKEYSDIALOG_H

#include <QDialog>
#include <QListView>
#include <QPushButton>

#include "keystatusparsererrormodel.h"

class RemoveExpiredKeysDialog : public QDialog
{
    Q_OBJECT
    using Error = eve::ApiKeyUpdater::Error;

  public:
    RemoveExpiredKeysDialog(const QVector<Error>& errors, QWidget* parent = nullptr);

  private:
    QListView* view_;
    eve::KeyStatusParserErrorModel* model_;

    QPushButton* removeButton_;
    QPushButton* cancelButton_;

  private slots:
    void removeSelectedKeys();
};

#endif // REMOVEEXPIREDKEYSDIALOG_H
