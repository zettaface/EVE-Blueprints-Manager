#include "bpfilter.h"

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

#include "../blueprintlist.h"

int streamOperatorsInit()
{
  qRegisterMetaTypeStreamOperators<BpFilter::Value>("BpFilter::Value");
  return 0;
}

int streamOperatorsInitVar = streamOperatorsInit();

bool BpFilter::validate(const QModelIndex& index) const
{

  if (index.parent().isValid()) {
    QVariant data = index.data(BlueprintList::BlueprintRole);
    const Blueprint bp = data.value<Blueprint>();
    return validate(bp);
  } else {
    QVariant data = index.data(BlueprintList::BlueprintHeaderRole);
    const BlueprintHeader bp = data.value<BlueprintHeader>();
    return validate(bp);
  }
}

int BpFilter::save(int startID, int parentID) const
{
  QSqlDatabase db = QSqlDatabase::database(":memory:");
  QSqlQuery q(db);

  q.prepare("INSERT INTO BlueprintFilters VALUES (?, ?, ?, ?)");
  q.addBindValue(startID);
  q.addBindValue(parentID);
  q.addBindValue(type_);
  q.addBindValue(val_);

  if (!q.exec())
    qWarning() << q.lastError();

  return ++startID;
}

QVariant BpFilter::value() const
{
  return val_;
}

void BpFilter::setValue(const QVariant& val)
{
  val_ = val;
  emit valueChanged();
}

int BpFilter::type() const
{
  return type_;
}

void BpFilter::setType(int type)
{
  type_ = type;
}

QDataStream& operator>>(QDataStream& in, BpFilter::Value& val)
{
  int first;
  int second;

  in >> first >> second;

  QPair<int, int> out(first, second);
  val = out;

  return in;
}

QDataStream& operator<<(QDataStream& out, const BpFilter::Value& val)
{
  out << val.first << val.second;

  return out;
}

QDebug operator<<(QDebug out, const BpFilter& bc)
{
  out << "BlueprintFilter(" << bc.displayData() << ")";
}
