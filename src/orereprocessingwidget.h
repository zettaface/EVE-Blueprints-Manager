#ifndef OREREPROCESSINGWIDGET_H
#define OREREPROCESSINGWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>

#include "blueprint.h"
#include "oretablemodel.h"

class OreReprocessingWidget : public QWidget
{
    Q_OBJECT
  public:
    explicit OreReprocessingWidget(QWidget* parent = 0);
    explicit OreReprocessingWidget(const Blueprint& bp, QWidget* parent = 0);

  private slots:
    void saveSettings() const;
    void restoreSettings() const;

    void getCompressedOre();
    void updateReprocessedMinerals();
    void validateOreSelection();
  private:
    struct SkillType
    {
      int id;
      QString name;
    };


    QVector<SkillType> reprocessingSkills() const;
    OreTableModel* oreReprocessingMode_;
    QTableView*    oreReprocessingView_;
    QTextEdit* oreResult;
    QPushButton* submit;

    QVector<QLineEdit*> mineralEdits_;
    QVector<QLineEdit*> mineralExtras_;
    QVector<QLabel*>    mineralAvailability_;
    QVBoxLayout*     resultLayout_;
    QVector<QLabel*> resultLabels_;
    QVector<bool>    selectedMinerals_;

    QComboBox* refinerySkillEditor;
    QComboBox* refineEfficiencySkillEditor;
    QComboBox* implantEditor;
    QComboBox* specImplantEditor;

    QMap<int, QComboBox*> skillEditors_;
    QVector<SkillType> reprocessingSkills_;

    QStringList miniralNames;
};

#endif // OREREPROCESSINGWIDGET_H
