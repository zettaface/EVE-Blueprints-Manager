#include "apikeyinsertdialog.h"
#include <QEventLoop>
#include <QLabel>
#include <QStackedWidget>
#include <QPushButton>
#include <QFormLayout>
#include <QGraphicsBlurEffect>
#include <QGraphicsColorizeEffect>
#include <QLineEdit>
#include <QCheckBox>
#include <QSplashScreen>

#include "eve/api.h"

class ApiKeyInsertDialog::CharSelectionPage : public QWizardPage
{
  public:
    CharSelectionPage(QWidget* parent = 0) : QWizardPage(parent) {
      setTitle("Character selection");
      setSubTitle("Select characters that you want to add:");

      QVBoxLayout* l = new QVBoxLayout(this);

      for (int i = 0; i < 3; i++) {
        QCheckBox* checkBox = new QCheckBox(this);
        l->addWidget(checkBox);
        checkboxes_.append(checkBox);
      }
    }

    void initializePage() {
      keylist_ = field("keys").value<QList<eve::ApiKeyInfo*>>();

      eve::ApiKeyList* loadedKeys = eve::API.keys();

      for (QCheckBox* checkbox : checkboxes_)
        checkbox->setVisible(false);

      int i = 0;
      for (eve::ApiKeyInfo* key : keylist_) {
        QCheckBox* checkBox = checkboxes_[i];
        QString displayName = key->charName();
        if (key->type() == "corp")
          displayName = key->corpName() + " (Corporation)";

        checkBox->setText(displayName);
        checkBox->setVisible(true);

        if (loadedKeys->contains(key)) {
          checkBox->setChecked(true);
          checkBox->setDisabled(true);
        } else {
          checkBox->setChecked(false);
          checkBox->setDisabled(false);
        }

        ++i;
      }
    }

    bool validatePage() {
      int i = 0;
      for (eve::ApiKeyInfo* key : keylist_) {
        QCheckBox* checkBox = checkboxes_[i];
        if (checkBox->isChecked() && checkBox->isEnabled() && checkBox->isVisible())
          eve::API.saveKey(key);

        ++i;
      }

      return true;
    }

  private:
    QLabel* test;
    QFormLayout* l;
    QList<eve::ApiKeyInfo*> keylist_;
    QVector<QCheckBox*> checkboxes_;
};

ApiKeyInsertDialog::ApiKeyInsertDialog(QWidget* parent) :
  QWizard(parent)
{
  addPage(new  ApiKeyPage);
  addPage(new CharSelectionPage);
  setWindowTitle("API Key insert");
}

void ApiKeyInsertDialog::showEvent(QShowEvent*)
{
  QPoint parentCenter = parentWidget()->geometry().center();
  QPoint sizeP(width() / 2, height() / 2);

  move(parentCenter - sizeP);
}

ApiKeyInsertDialog::ApiKeyPage::ApiKeyPage(QWidget* parent) :
  QWizardPage(parent)
{
  setTitle(tr("API key"));



  QString message  = tr("Your API keys available <a href=\"https://community.eveonline.com/support/api-key/\">here</a><br/>"
                        "You can create predefined API key "
                        "<a href=\"https://community.eveonline.com/support/api-key/createpredefined?accessmask=134283394\" >here</a>");
  QLabel* infoLabel = new QLabel(message, this);
  infoLabel->setOpenExternalLinks(true);

  keyLabel = new QLabel("Key ID:", this);
  keyEdit = new QLineEdit(this);
  keyLabel->setBuddy(keyEdit);
  serverMessage_ = new QLabel(this);

  vCodeLabel = new QLabel("Verification code:", this);
  vCodeEdit = new QLineEdit(this);
  vCodeLabel->setBuddy(vCodeEdit);

  registerField("keyID*", keyEdit);
  registerField("vCode*", vCodeEdit);
  registerField("keys", this, "keylist");

  QFormLayout* l = new QFormLayout(this);
  l->addRow(infoLabel);
  l->addRow(keyLabel, keyEdit);
  l->addRow(vCodeLabel, vCodeEdit);
  l->addRow(serverMessage_);
}

bool ApiKeyInsertDialog::ApiKeyPage::validatePage()
{
  QString keyID = field("keyID").toString();
  QString vCode = field("vCode").toString();

  {
    QSharedPointer<eve::KeyInfoXmlParser> parser = QSharedPointer<eve::KeyInfoXmlParser>::create();

    QEventLoop loop;
    QSplashScreen splash;

    connect(parser.data(), &eve::KeyInfoXmlParser::finished, this, &ApiKeyPage::setKeyList);
//    connect(parser.data(), SIGNAL(finished(QList<ApiKeyInfo*>)), this, SLOT(setKeyList(QList<eve::ApiKeyInfo*>)));
    connect(parser.data(), SIGNAL(finished(QList<ApiKeyInfo*>)), &splash, SLOT(close()));
    connect(parser.data(), SIGNAL(finished(QList<ApiKeyInfo*>)), &loop, SLOT(quit()));
    eve::API.fetchKeys(keyID, vCode, parser);

    setEnabled(false);
    splash.show();
    loop.exec();
    setEnabled(true);

    if (parser->error() > 0)
      serverMessage_->setText(parser->errorString());
  }

  if (_keylist.empty())
    return false;

  setField("keys", QVariant::fromValue<QList<eve::ApiKeyInfo*>>(_keylist));
  return true;
}

QList<eve::ApiKeyInfo*> ApiKeyInsertDialog::ApiKeyPage::getKeyList() const
{
  return _keylist;
}

void ApiKeyInsertDialog::ApiKeyPage::setKeyList(QList<eve::ApiKeyInfo*> keylist)
{
  _keylist = keylist;
}
