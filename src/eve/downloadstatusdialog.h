#ifndef DOWNLOADSTATUSDIALOG_H
#define DOWNLOADSTATUSDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

#include "parser/assetxmlparser.h"
#include "parser/parser.h"

class DownloadStatusDialog : public QDialog
{
    Q_OBJECT
  public:
    DownloadStatusDialog(QWidget* parent = nullptr);

  public slots:
    void reject() override;
    void reset();

  private slots:
    void startDownload();

  private:
    QWidget* makeKeysStatusWidget();
    QWidget* makeDownloadStatusWidget();
    QWidget* makeStatusLabel(eve::ApiKeyInfo* key);
    void connectParser(QSharedPointer<eve::XmlParser> parser, QProgressBar* progressBar);
    void connectAssetParser(QSharedPointer<eve::AssetXmlParser> assetsParser, QProgressBar* progressBar);

    QProgressBar* assetProgressBar_;
    QProgressBar* industryJobsProgressBar_;
    QProgressBar* blueprintProgressBar_;
    QProgressBar* locationProgressBar_;

    bool downloading_ { false };
};

#endif // DOWNLOADSTATUSDIALOG_H
