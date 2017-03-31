#include "inventorytypeimageparser.h"

#include <QImage>
#include <QtSql>

#include "../imageserverrequest.h"

void eve::InventoryTypeImageParser::parse(QByteArray& raw, Request* req)
{
  QImage image = QImage::fromData(raw);
  if (image.isNull()) {
    qWarning() << "Invalid image recieved";
    return;
  }

  QSqlDatabase db = QSqlDatabase::database(":memory:");
  QSqlQuery q(db);

  ImageServerRequest* imReq = static_cast<ImageServerRequest*>(req);

  q.prepare("INSERT OR iGNORE INTO InventoryTypeImages VALUES (?, ?)");
  q.addBindValue(QVariant::fromValue<long>(imReq->id()));
  q.addBindValue(raw);

  if (!q.exec())
    qWarning() << q.lastError();
}
