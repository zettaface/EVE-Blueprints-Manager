#ifndef KEYSTATUSPARSERERRORMODEL_H
#define KEYSTATUSPARSERERRORMODEL_H

#include <QAbstractListModel>
#include "parser/apikeyupdater.h"

namespace eve {

class KeyStatusParserErrorModel : public QAbstractListModel
{
    using Error = ApiKeyUpdater::Error;
    class Element : public ApiKeyUpdater::Error {
      public:
        Element() = default;
        Element(const Error& e) : Error(e) {}
        Qt::CheckState checked_ { Qt::Checked };
    };

  public:
    KeyStatusParserErrorModel(QObject* parent);
    KeyStatusParserErrorModel(const QVector<Error>& errors, QObject* parent);

    void setData(const QVector<Error>& errors);

    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    int columnCount(const QModelIndex& parent) const override;

    enum Columns {
      Name = 0,
      ErrorMessage,
      ColumnCount
    };

    enum Role {
      ID = Qt::UserRole
    };

  private:
    QVector<Element> data_;
};

}

#endif // KEYSTATUSPARSERERRORMODEL_H
