#ifndef REMOVEDUPLICATEKEYSDIALOG_H
#define REMOVEDUPLICATEKEYSDIALOG_H

#include <QDialog>
#include <QSet>

#include "apikey.h"

class RemoveDuplicateKeysDialog : public QDialog
{
    Q_OBJECT
  public:
    explicit RemoveDuplicateKeysDialog(QWidget* parent = nullptr);

    bool hasDuplicates() const { return duplicates_.size(); }
  private:
    QVector<QVector<eve::ApiKeyInfo*>> findDuplicates() const;

    QVector<QVector<eve::ApiKeyInfo*>> duplicates_;
    QSet<int> removeList_;
  private slots:
    void removeDuplicates();
};

#endif // REMOVEDUPLICATEKEYSDIALOG_H
