#include "downloadmanager.h"

#include <QFileInfo>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QDebug>


namespace eve {

DownloadManager::DownloadManager(QObject* parent)
  : QObject(parent), requestCount(0)
{

}

void DownloadManager::append(Request* req)
{
  if (downloadQueue.isEmpty() && !currentRequest)
    QTimer::singleShot(0, this, SLOT(startNextDownload()));

  downloadQueue.append(req);
}

void DownloadManager::prepend(Request* req)
{
  if (downloadQueue.isEmpty() && !currentRequest)
    QTimer::singleShot(0, this, SLOT(startNextDownload()));

  downloadQueue.prepend(req);
}

void DownloadManager::startNextDownload()
{
  if (downloadQueue.isEmpty()) {
    emit finished();
    return;
  }

  download(downloadQueue.dequeue());
}

void DownloadManager::download(Request* req)
{
  currentRequest = req;
  QUrl url(currentRequest->url());

  QNetworkRequest request(url);
  currentDownload = manager.get(request);
  connect(currentDownload, SIGNAL(finished()),
          SLOT(downloadFinished()));

  emit processing(requestCount, downloadQueue.size());
  downloadTime.start();
}

void DownloadManager::downloadFinished()
{
//  QSharedPointer<Parser> parser = currentRequest->parse();

//  if (!parser) {
//    qWarning() << "No parser for " << currentRequest->url();
//    currentDownload->deleteLater();
//    startNextDownload();
//    return;
//  }

  if (currentDownload->error())
    qWarning() << "Download failed: " << currentDownload->errorString();

  QByteArray result = currentDownload->readAll();
  currentRequest->parse(result);

  requestCount++;
  currentDownload->deleteLater();
  delete currentRequest;
  currentRequest = nullptr;
  startNextDownload();
}

void DownloadManager::removeAll(const QString& req)
{
  qWarning() << "\"" << req << "\"" << "Too much errors. Skipping all";
  auto it = std::remove_if(downloadQueue.begin(), downloadQueue.end(), [req](Request * r) {
    if (r->path() == req)
      return true;

    return false;
  });

  downloadQueue.erase(it, downloadQueue.end());
}

Request* DownloadManager::getCurrentRequest() const
{
  return currentRequest;
}

} // namespace eve
