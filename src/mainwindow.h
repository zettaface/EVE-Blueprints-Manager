#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QVBoxLayout>

#include "blueprintswidget.h"
#include "eve/downloadstatusdialog.h"

class BlueprintList;

class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow() = default;

  private:
    void buildMenu();

    QLabel* statusBarMessage;
    BlueprintsWidget* bpView;
    BlueprintList* blueprintsModel;
  signals:

  public slots:
    void updateStatusMessage();

  private slots:
    void updateApi();
};

Q_DECLARE_METATYPE(QList<int>)

#endif // MAINWINDOW_H
