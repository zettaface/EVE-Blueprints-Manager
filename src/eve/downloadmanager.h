#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QFile>
#include <QMap>
#include <QObject>
#include <QQueue>
#include <QTime>
#include <QUrl>
#include <QNetworkAccessManager>

#include <queue>

#include "apikey.h"
#include "request.h"
#include "parser/parser.h"

namespace eve {

class DownloadManager: public QObject
{
    Q_OBJECT
  public:
    DownloadManager(QObject* parent = 0);

    void installParser(QString req, Parser* parser);
    void append(Request* req);
    void prepend(Request* req);

    Request* getCurrentRequest() const;

  signals:
    void finished();
    void processing(int, int);

  private slots:
    void startNextDownload();
    void downloadFinished();

  private:
    void download(Request* req);
    void removeAll(const QString& req);

    const int maxErrors = 3;
    QMap<QString, Parser*> handlers;
    QMap<QString, int> handlerErrorsCount;
    QNetworkAccessManager manager;
    QQueue<Request*> downloadQueue;
    QNetworkReply* currentDownload;
    Request* currentRequest;
    QTime downloadTime;

    int requestCount {0};
};

} // namespace eve

#endif
