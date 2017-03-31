#ifndef APIKEYINSERTDIALOG_H
#define APIKEYINSERTDIALOG_H

#include <QObject>
#include <QDialog>
#include <QWizard>

#include <QtWidgets>

#include "eve/apikey.h"

class ApiKeyInsertDialog : public QWizard
{
    Q_OBJECT
  public:
    ApiKeyInsertDialog(QWidget* parent = 0);
    ~ApiKeyInsertDialog() = default;

  protected:
    void showEvent(QShowEvent*);

  private:
    class ApiKeyPage;
    class CharSelectionPage;
};

class ApiKeyInsertDialog::ApiKeyPage : public QWizardPage
{
    Q_OBJECT
    Q_PROPERTY(QList<eve::ApiKeyInfo*> keylist READ getKeyList WRITE setKeyList)
  public:
    ApiKeyPage(QWidget* parent = 0);

    bool validatePage();
    QList<eve::ApiKeyInfo*> getKeyList() const;

  public slots:
    void setKeyList(QList<eve::ApiKeyInfo*> keylist);

  private:
    QLabel* keyLabel;
    QLabel* vCodeLabel;
    QLineEdit* keyEdit;
    QLineEdit* vCodeEdit;

    QList<eve::ApiKeyInfo*> _keylist;
};

#endif // APIKEYINSERTDIALOG_H
