#include "mainwindow.h"

#include <QTabWidget>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QDialog>
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QCompleter>
#include <QProgressBar>
#include <QStackedWidget>
#include <QSharedPointer>
#include <QSettings>
#include <QFileDialog>
#include <QPropertyAnimation>

#include <fstream>

#include "apikeyinsertdialog.h"
#include "apikeymanagmentdialog.h"
#include "assettree.h"
#include "blueprintlist.h"
#include "blueprintpage.h"
#include "bpcolordialog.h"
#include "bpview.h"
#include "categoryfilterproxymodel.h"
#include "eve/api.h"
#include "eve/staticdata.h"
#include "hierarchicalcombobox.h"
#include "marketidfiltermodel.h"
#include "orereprocessingwidget.h"
#include "productionmodel.h"
#include "eve/removeexpiredkeysdialog.h"
#include "eve/updateapikeysdialog.h"
#include "eve/removeduplicatekeysdialog.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
  qRegisterMetaType<QList<int>>();
  eve::API.init();
  eve::SDE.init();

  statusBarMessage = new QLabel(this);
  bpView = new BlueprintsWidget(this);
  blueprintsModel = bpView->model();
  buildMenu();

  setCentralWidget(bpView);

  connect(bpView, SIGNAL(modelUpdated()), this, SLOT(updateStatusMessage()));

  updateStatusMessage();
  setWindowTitle(tr("Blueprint Manager"));
  statusBar()->addWidget(statusBarMessage);
}

void MainWindow::buildMenu()
{
  QAction* addApiKeyA = new QAction("&Add API key...", this);
  connect(addApiKeyA, &QAction::triggered, [this]() {
    ApiKeyInsertDialog dial(this);
    dial.exec();
  });

  QAction* manageApiKeysA = new QAction("&Manage API keys...", this);
  connect(manageApiKeysA, &QAction::triggered, [this]() {
    ApiKeyManagmentDialog dial(this);
    dial.exec();
  });

  QAction* manageColorsA = new QAction("&Colors...", this);
  connect(manageColorsA, &QAction::triggered, [this]() {
    BPColorDialog dial(bpView->colorModel()->bpColors(), this);
    int result = dial.exec();
    if (result == QDialog::Accepted)
      bpView->clearCache();
  });

  const QString aboutText = tr(
                              "<b>EVE Blueprint Manager ver. 0.1</b><br/><br/>"
                              "For any questions post me ingame mail on Anna Spade<br/><br/>"
                              "The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE "
                              "WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.<br/><br/>"
                              "EVE Blueprint Manager released under GNU GPLv3<br/>"
                              "All EVE-Online related materials are property of CCP hf.<br/><br/>"
                              "Qt ver.: "
                              QT_VERSION_STR
                            );

  QAction* aboutA = new QAction("&About", this);
  connect(aboutA, &QAction::triggered, [this, aboutText]() {
    QMessageBox aboutBox(this);
    aboutBox.setIconPixmap(QPixmap(":/icons/icon_64"));
    aboutBox.setWindowTitle("EVE Blueprint Manager");
    aboutBox.setText(aboutText);

    aboutBox.exec();
  });

  QAction* updateApiA = new QAction("U&pdate API Data", this);
  connect(updateApiA, SIGNAL(triggered(bool)), this, SLOT(updateApi()));

  QAction* clearCacheA = new QAction("C&lear Cache", this);
  connect(clearCacheA, SIGNAL(triggered(bool)), &eve::API, SLOT(clearCache()));
  connect(clearCacheA, SIGNAL(triggered(bool)), blueprintsModel, SLOT(loadFromMemory()));

  QAction* exitA = new QAction("&Exit", this);
  connect(exitA, SIGNAL(triggered()), this, SLOT(close()));

  QAction* oreProcessingA = new QAction("&Compressed ore calculator", this);
  connect(oreProcessingA, &QAction::triggered, []() {
    QWidget* tmp = new OreReprocessingWidget;
    tmp->setMinimumSize(600, 400);
    tmp->show();
  });

  QMenu* fileMenu = menuBar()->addMenu("&File");
  QMenu* viewMenu = menuBar()->addMenu("&Tools");
  QMenu* helpMenu = menuBar()->addMenu("&Help");
  fileMenu->addAction(addApiKeyA);
  fileMenu->addAction(manageApiKeysA);
  fileMenu->addSeparator();
  fileMenu->addAction(manageColorsA);
  fileMenu->addSeparator();
  fileMenu->addAction(updateApiA);
  fileMenu->addAction(clearCacheA);
  fileMenu->addSeparator();
  fileMenu->addAction(exitA);

  viewMenu->addAction(oreProcessingA);

  helpMenu->addAction(aboutA);
}

void MainWindow::updateStatusMessage()
{
  statusBarMessage->setText(bpView->aggregationInfo());
}

void MainWindow::updateApi()
{
  UpdateApiKeysDialog keyUpdateDialog;
  keyUpdateDialog.exec();

  if (keyUpdateDialog.expiredKeys().size() > 0) {
    RemoveExpiredKeysDialog expiredKeysRemoveDialog(keyUpdateDialog.expiredKeys());
    int status = expiredKeysRemoveDialog.exec();

    if (status == QDialog::Accepted)
      eve::API.keys()->fetchFromDB();
  }

  RemoveDuplicateKeysDialog duplicatesDialog;
  if (duplicatesDialog.hasDuplicates()) {
    int status = duplicatesDialog.exec();

    if (status == QDialog::Accepted)
      eve::API.keys()->fetchFromDB();
  }

  DownloadStatusDialog* dialog = new DownloadStatusDialog(this);

  connect(dialog, SIGNAL(accepted()), dialog, SLOT(deleteLater()));
  connect(dialog, SIGNAL(accepted()), blueprintsModel, SLOT(loadFromMemory()));
  connect(dialog, SIGNAL(rejected()), dialog, SLOT(deleteLater()));
  dialog->exec();
}
