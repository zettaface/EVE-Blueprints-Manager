#ifndef APIKEYMANAGMENTDIALOG_H
#define APIKEYMANAGMENTDIALOG_H

#include <QObject>
#include <QDialog>
#include <QTreeView>
#include <QPushButton>

class ApiKeyManagmentDialog : public QDialog
{
    Q_OBJECT
  public:
    ApiKeyManagmentDialog(QWidget* parent = 0);
    ~ApiKeyManagmentDialog() = default;
  private:
    QTreeView* view;
    QPushButton* addKeysBtn;
    QPushButton* closeBtn;
  private slots:
    void showContextMenu(QPoint pos);
};

#endif // APIKEYMANAGMENTDIALOG_H
