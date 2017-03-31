#include "blueprintsdecorationproxy.h"

#include <QDebug>
#include <QPainter>
#include <QPair>
#include <QPixmap>

#include <algorithm>

#include "blueprintlist.h"
#include "eve/staticdata.h"

BlueprintsDecorationProxy::BlueprintsDecorationProxy(QObject* parent) :
  QIdentityProxyModel(parent)
{
  initActivityIcons();
}

QVariant BlueprintsDecorationProxy::data(const QModelIndex& index, int role) const
{
  const bool isHeader = !index.parent().isValid();

  if (!index.isValid()) {
    qDebug() << "Broken index";
    return QIdentityProxyModel::data(index, role);
  }

  if (role == Qt::DisplayRole) {
    if (index.column() == Blueprint::Activity)
      return "";
  }

  if (role == Qt::DecorationRole) {
    if (index.column() == Blueprint::Name) {
      if (isHeader) {
        if (decorations.contains(index.row()))
          return decorations[index.row()];

        const QAbstractItemModel* m1 = sourceModel();
        const QAbstractItemModel* m2 = index.model();

        QPixmap d = makeDecoration((index));
        decorations[index.row()] = d;

        return d;
      }
    } else if (index.column() == Blueprint::Activity) {
      int activity = index.data(Qt::UserRole).toInt();
      if (activity == 0)
        return QVariant();

      return activityIcons[activity];
    }
  }

  return QIdentityProxyModel::data(index, role);
}

void BlueprintsDecorationProxy::clearCache()
{
  qDebug() << "decorations clear cache";
  beginResetModel();
  decorations.clear();
  endResetModel();
}

QPixmap BlueprintsDecorationProxy::makeDecoration(const QModelIndex& index) const
{
  const double pi = std::acos(-1);
  const int outputWidth = 16;
  const int width = outputWidth / cos(pi / 4);
  const int height = width;

  const QAbstractItemModel* model = this;

  QVector<QColor> colors;
  int total = 0;

  int childRows = model->rowCount(index);

  for (int i = 0; i < childRows; i++) {
    QColor c = model->index(i, 1, index).data(Qt::BackgroundRole).value<QColor>();
    colors.push_back(c);
  }

  total = colors.size();

  QPixmap pie = QPixmap(outputWidth, outputWidth);
  QRect rect(0, 0, width, height);
  rect.moveCenter(QRect(0, 0, outputWidth, outputWidth).center());
  QPainter p(&pie);

  QColor innerBorder = QColor(Qt::black);
  p.setPen(innerBorder);

  double startAngle = 0;

  if (colors.isEmpty()) {
    QColor c = index.data(Qt::BackgroundRole).value<QColor>();
    p.setBrush(c);
    p.setPen(QPen(Qt::black, 1));
    p.drawRect(0, 0, pie.width() - 1, pie.height() - 1);

    return pie;
  }

  if (colors.size() == 1) {
    p.setBrush(colors[0]);
    p.setPen(QPen(Qt::black, 1));
    p.drawRect(0, 0, pie.width() - 1, pie.height() - 1);

    return pie;
  }

  if (colors.size() >= 10)
    p.setPen(Qt::NoPen);

  for (auto& color : colors) {
    double spanAngle = 360.0 * (1.0 / total);
    if (spanAngle <= 0)
      spanAngle = 1;
    if (startAngle + spanAngle > 360)
      spanAngle = 360 - startAngle + 1;

    p.setBrush(color);
    p.drawPie(rect, (startAngle - 90) * 16, spanAngle * 16);

    startAngle += spanAngle;
  }

  p.setBrush(Qt::NoBrush);
  p.setPen(QPen(Qt::black, 1));
  p.drawRect(0, 0, pie.width() - 1, pie.height() - 1);

  return pie;
}

void BlueprintsDecorationProxy::initActivityIcons()
{
  QMap<int, QString> paths {
    {1, ":icons/manufacturing"},
    {2, ":icons/invention"},
    {3, ":icons/researchTE"},
    {4, ":icons/researchME"},
    {5, ":icons/copying"},
    {6, ":icons/copying"},
    {7, ":icons/invention"},
    {8, ":icons/invention"}
  };

  for (int i = 1; i <= 8; i++) {
    QImage img(paths[i]);
    if (img.isNull())
      qDebug() << "null image" << paths[i];

    QPixmap image = QPixmap::fromImage(img);
    activityIcons[i] = image;
  }
}


void BlueprintsDecorationProxy::setSourceModel(QAbstractItemModel* sourceModel)
{
  QAbstractItemModel* oldSource = this->sourceModel();
  if (oldSource)
    disconnect(oldSource, 0, this, 0);

  QIdentityProxyModel::setSourceModel(sourceModel);
  clearCache();
}
