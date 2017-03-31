#include "keystatusparsererrormodel.h"

namespace eve {


KeyStatusParserErrorModel::KeyStatusParserErrorModel(QObject* parent) :
  QAbstractListModel(parent)
{

}

KeyStatusParserErrorModel::KeyStatusParserErrorModel(const QVector<Error>& errors, QObject* parent) :
  KeyStatusParserErrorModel(parent)
{
  setData(errors);

}

void KeyStatusParserErrorModel::setData(const QVector<KeyStatusParserErrorModel::Error>& errors)
{
  beginResetModel();
  data_.clear();

  for (auto& e : errors)
    data_.push_back(e);

  endResetModel();
}


int KeyStatusParserErrorModel::rowCount(const QModelIndex& parent) const
{
  return data_.size();
}

QVariant KeyStatusParserErrorModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case Name:
        return data_[index.row()].key->name();
        break;
      case ErrorMessage:
        return QString("(%1) %2").arg(data_[index.row()].errorCode).arg(data_[index.row()].errorInfo);
        break;
      default:
        break;
    }
  } else if (role == Qt::CheckStateRole) {
    switch(index.column()) {
      case Name:
        return data_[index.row()].checked_;
        break;
      default:
        break;
    }
  } else if (role == ID)
    return data_[index.row()].key->ID();

  return QVariant();
}

bool KeyStatusParserErrorModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (role == Qt::CheckStateRole) {
    switch(index.column()) {
      case Name:
        data_[index.row()].checked_ = value.value<Qt::CheckState>();
        return true;
      default:
        break;
    }
  }

  return QAbstractListModel::setData(index, value, role);
}

Qt::ItemFlags KeyStatusParserErrorModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
    return QAbstractListModel::flags(index);

  if (index.column() == Name)
    return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
  else
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  return QAbstractListModel::flags(index);
}

int KeyStatusParserErrorModel::columnCount(const QModelIndex& parent) const
{
  return ColumnCount;
}

} //namespace eve
