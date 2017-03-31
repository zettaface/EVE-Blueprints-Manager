#include "downloadstatusdialog.h"
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QStackedWidget>
#include <QScrollArea>
#include <QKeyEvent>

#include "removeexpiredkeysdialog.h"
#include "api.h"
#include "parser/assetxmlparser.h"
#include "parser/blueprintsxmlparser.h"
#include "parser/industryjobsxmlparser.h"

DownloadStatusDialog::DownloadStatusDialog(QWidget* parent) :
  QDialog(parent, Qt::WindowTitleHint)
{
  setWindowTitle("API Keys status");

  QWidget* keysStatusWidget = makeKeysStatusWidget();
  QWidget* downloadStatusContainer = makeDownloadStatusWidget();

  QStackedWidget* w = new QStackedWidget(this);
  w->addWidget(keysStatusWidget);
  w->addWidget(downloadStatusContainer);

  QFrame* line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  QDialogButtonBox* buttons = new QDialogButtonBox(this);
  buttons->addButton(QDialogButtonBox::StandardButton::Ok);
  buttons->addButton(QDialogButtonBox::StandardButton::Cancel);
  connect(buttons, SIGNAL(accepted()), this, SLOT(startDownload()));
  connect(buttons, &QDialogButtonBox::accepted, [buttons, w]() {
    w->setCurrentIndex(1);
    buttons->setDisabled(true);
  });

  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout* l = new QVBoxLayout(this);
  l->addWidget(w, 1);
  l->addWidget(line);
  l->addWidget(buttons);

  setMinimumWidth(380);
  reset();
}

QWidget* DownloadStatusDialog::makeKeysStatusWidget()
{
  QScrollArea* keysStatusArea = new QScrollArea(this);
  QWidget* cont = new QWidget(this);
  QGridLayout* layout = new QGridLayout;
  layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  layout->setSpacing(5);
  layout->setColumnStretch(0, 0);
  layout->setColumnStretch(1, 1);

  cont->setLayout(layout);
  keysStatusArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  keysStatusArea->setWidgetResizable(true);

  int i = 0;
  for (auto key : *eve::API.keys()) {
    QString title = QString("%1. %2").arg(i + 1).arg(key->name());
    QLabel* keyLabel = new QLabel(title, this);
    QWidget* statusLabel = makeStatusLabel(key);

    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMinimumSize(320, 3);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    layout->addWidget(keyLabel, 2 * i, 0, 1, 1, Qt::AlignTop | Qt::AlignLeft);
    layout->addWidget(statusLabel, 2 * i, 1, 1, 1, Qt::AlignTop | Qt::AlignLeft);
    layout->addWidget(line, 2 * i + 1, 0, 1, 3, Qt::AlignLeft);
    i++;
  }

  QVBoxLayout* expandingLayout = new QVBoxLayout;
  expandingLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
  layout->addLayout(expandingLayout, 2 * i, 0, 1, 2);

  keysStatusArea->setWidget(cont);

  return keysStatusArea;
}

QWidget* DownloadStatusDialog::makeDownloadStatusWidget()
{
  QLabel* assetLabel_ = new QLabel("Assets:");
  assetProgressBar_ = new QProgressBar(this);

  QLabel* industryJobsLabel_ = new QLabel("Industry Jobs:");
  industryJobsProgressBar_ = new QProgressBar(this);

  QLabel* blueprintLabel_ = new QLabel("Blueprints:");
  blueprintProgressBar_ = new QProgressBar(this);

  QLabel* locationLabel_ = new QLabel("Locations:");
  locationProgressBar_ = new QProgressBar(this);

  QFormLayout* mainLayout = new QFormLayout();
  mainLayout->addRow(assetLabel_, assetProgressBar_);
  mainLayout->addRow(blueprintLabel_, blueprintProgressBar_);
  mainLayout->addRow(industryJobsLabel_, industryJobsProgressBar_);
  mainLayout->addRow(locationLabel_, locationProgressBar_);

  QWidget* downloadStatusContainer = new QWidget(this);
  QVBoxLayout* downloadStatusLayout = new QVBoxLayout(downloadStatusContainer);
  downloadStatusLayout->addLayout(mainLayout);
  downloadStatusLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

  return downloadStatusContainer;
}

QWidget* DownloadStatusDialog::makeStatusLabel(eve::ApiKeyInfo* key)
{
  auto format = [](const QDateTime & time) -> QString {
    if (time <= QDateTime::currentDateTimeUtc())
      return "<font color=\"Green\">Available</font>";

    return QString("<font color=\"Red\">Cached until %1</font>")
    .arg(time.toLocalTime().toString(Qt::SystemLocaleShortDate));
  };

  // multiline QLabel didn't work in QScrollArea for some reason
  QVBoxLayout* layout = new QVBoxLayout;
  layout->setSpacing(1);
  layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  layout->setMargin(0);

  QWidget* container = new QWidget();
  container->setLayout(layout);

  QVector<QPair<QString, QDateTime>> labels {
    {"Assets", key->assetCachedUntil_},
    {"Blueprints", key->blueprintsCachedUntil_},
    {"Industry jobs", key->industryJobsCachedUntil_}
  };

  for (auto p : labels) {
    QString labelText = QString("%1 - %2").arg(p.first, format(p.second));
    QLabel* label = new QLabel(labelText, container);

    layout->addWidget(label);
  }

  container->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  return container;
}

void DownloadStatusDialog::reset()
{
  assetProgressBar_->setRange(0, 0);
  blueprintProgressBar_->setRange(0, 0);
  industryJobsProgressBar_->setRange(0, 0);
  locationProgressBar_->setRange(0, 0);

  assetProgressBar_->setValue(0);
  blueprintProgressBar_->setValue(0);
  industryJobsProgressBar_->setValue(0);
  locationProgressBar_->setValue(0);
}

void DownloadStatusDialog::startDownload()
{
  using namespace eve;

  setWindowTitle("Downloading...");
  setFixedSize(300, 160);

  auto assetsParser = QSharedPointer<AssetXmlParser>::create();
  auto blueprintsParser = QSharedPointer<BlueprintsXmlParser>::create();
  auto industryJobsParser = QSharedPointer<IndustryJobsXmlParser>::create();

  connectAssetParser(assetsParser, assetProgressBar_);
  connectParser(blueprintsParser, blueprintProgressBar_);
  connectParser(industryJobsParser, industryJobsProgressBar_);

  int assetsToFetch = eve::API.queueAssetsFetch(assetsParser);
  int blueprintsToFetch = eve::API.queueBlueprintsFetch(blueprintsParser);
  int industryJobsToFetch = eve::API.queueIndustryJobsFetch(industryJobsParser);

  qDebug() << assetsToFetch << blueprintsToFetch << industryJobsToFetch;

  if (assetsToFetch || blueprintsToFetch || industryJobsToFetch) {
    downloading_ = true;

    assetProgressBar_->setRange(0, assetsToFetch);
    blueprintProgressBar_->setRange(0, blueprintsToFetch);
    industryJobsProgressBar_->setRange(0, industryJobsToFetch);

    connect(&eve::API, SIGNAL(fetchFinished()), this, SLOT(accept()));
  } else
    reject();
}

void DownloadStatusDialog::connectAssetParser(QSharedPointer<eve::AssetXmlParser> assetsParser, QProgressBar* progressBar)
{
  using namespace eve;

  connect(assetsParser.data(), &AssetXmlParser::parsed, [this, progressBar](ApiKeyInfo * key) {
    using namespace eve;

    progressBar->setValue(progressBar->value() + 1);
    auto locParser = QSharedPointer<LocationsXmlParser>::create();

    connect(locParser.data(), &LocationsXmlParser::parsed, [this]() {
      locationProgressBar_->setValue(locationProgressBar_->value() + 1);
    });

    int locCount = API.queueLocationsFetch(key, locParser);
    locationProgressBar_->setMaximum(locationProgressBar_->maximum() + locCount);
  });
}

void DownloadStatusDialog::connectParser(QSharedPointer<eve::XmlParser> parser, QProgressBar* progressBar)
{
  using namespace eve;

  connect(parser.data(), &XmlParser::parsed, [progressBar](ApiKeyInfo * key) {
    progressBar->setValue(progressBar->value() + 1);
  });
}

void DownloadStatusDialog::reject()
{
  if (!downloading_)
    QDialog::reject();
}
