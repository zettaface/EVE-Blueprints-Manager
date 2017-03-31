#include "orereprocessingwidget.h"

#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QStackedLayout>
#include <QTextEdit>
#include <QtSql>

#include "simplex.h"
#include "oreselectionwidget.h"

OreReprocessingWidget::OreReprocessingWidget(QWidget* parent) :
  OreReprocessingWidget(Blueprint(), parent)
{

}

OreReprocessingWidget::OreReprocessingWidget(const Blueprint& bp, QWidget* parent) : QWidget(parent),
  selectedMinerals_(7, false)
{
  QSettings settings;

  miniralNames = QStringList {
    "Tritanium",
    "Pyerite",
    "Mexallon",
    "Isogen",
    "Nocxium",
    "Zydrine",
    "Megacyte"
  };

  OreSelectionWidget* oresView = new OreSelectionWidget(this);
  oreReprocessingView_ = new QTableView(this);

  oreReprocessingMode_ = new OreTableModel(this);
  oreReprocessingMode_->loadFromMemory();

  oreReprocessingView_->setModel(oreReprocessingMode_);
  oreReprocessingView_->resizeColumnsToContents();

  QRadioButton* detailedViewRadio = new QRadioButton("Detailed view");
  QRadioButton* compactViewRadio = new QRadioButton("Compact view");

  QHBoxLayout* oreViewRadioLayout = new QHBoxLayout;
  oreViewRadioLayout->addWidget(detailedViewRadio);
  oreViewRadioLayout->addWidget(compactViewRadio);
  QVBoxLayout* oreViewLayout = new QVBoxLayout;

  QStackedLayout* oreViewInnerLayout = new QStackedLayout;
  oreViewInnerLayout->addWidget(oreReprocessingView_);
  oreViewInnerLayout->addWidget(oresView);

  oreViewLayout->addLayout(oreViewRadioLayout);
  oreViewLayout->addLayout(oreViewInnerLayout);

  QGroupBox* oresViewGroup = new QGroupBox("Ores", this);
  oresViewGroup->setLayout(oreViewLayout);
  oresViewGroup->setContentsMargins(10, 15, 10, 10);

  connect(detailedViewRadio, &QRadioButton::toggled, [oreViewInnerLayout]() {
    oreViewInnerLayout->setCurrentIndex(0);
  });

  connect(compactViewRadio, &QRadioButton::toggled, [oreViewInnerLayout]() {
    oreViewInnerLayout->setCurrentIndex(1);
  });

  oresView->setModel(oreReprocessingMode_);

  // skills block

  QGroupBox* skillsGroup = new QGroupBox("Skill and Implants", this);
  QGridLayout* skillsLayout = new QGridLayout;

  QStringList skillLevels {"None"};
  for (int i = 1; i <= 5; i++)
    skillLevels.append(QString("Level %1").arg(i));

  skillsGroup->setLayout(skillsLayout);

  QLabel*    refinerySkillLabel  = new QLabel("Reprocessing");
  refinerySkillEditor = new QComboBox(this);
  refinerySkillEditor->addItems(skillLevels);

  QLabel*    refineEfficiencySkillLabel  = new QLabel("Reprocessing Efficiency");
  refineEfficiencySkillEditor = new QComboBox(this);
  refineEfficiencySkillEditor->addItems(skillLevels);

  QLabel*    implantLabel  = new QLabel("'Highwall' Mining Implant");
  implantEditor = new QComboBox(this);
  implantEditor->addItems({"None", "MX-1001", "MX-1003", "MX-1005"});

  QLabel*    specImplantLabel  = new QLabel("Special Implant");
  specImplantEditor = new QComboBox(this);
  specImplantEditor->addItems({"None", "Michi's Excavation Augmentor"});


  skillsLayout->addWidget(refinerySkillLabel, 0, 0);
  skillsLayout->addWidget(refinerySkillEditor, 1, 0);
  skillsLayout->addWidget(refineEfficiencySkillLabel, 2, 0);
  skillsLayout->addWidget(refineEfficiencySkillEditor, 3, 0);
  skillsLayout->addWidget(implantLabel, 4, 0);
  skillsLayout->addWidget(implantEditor, 5, 0);
  skillsLayout->addWidget(specImplantLabel, 6, 0);
  skillsLayout->addWidget(specImplantEditor, 7, 0);

  reprocessingSkills_ = reprocessingSkills();
  const int skillsPerLine = 4;
  int i = 0;
  for (const SkillType& s : reprocessingSkills_) {
    QLabel* skillLabel = new QLabel(s.name, this);
    QComboBox* lvlEditor = new QComboBox(this);
    lvlEditor->addItems(skillLevels);

    int currentIndex = settings.value(QString("reprocessingSkills/%1").arg(s.id), 5).toInt();
    lvlEditor->setCurrentIndex(currentIndex);

    skillsLayout->addWidget(skillLabel, 2 * (i / skillsPerLine),     i % skillsPerLine + 1);
    skillsLayout->addWidget(lvlEditor,  2 * (i / skillsPerLine) + 1, i % skillsPerLine + 1);

    skillEditors_[s.id] = lvlEditor;

    connect(lvlEditor, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [s](int index) {
      QSettings settings;
      settings.setValue(QString("reprocessingSkills/%1").arg(s.id), index);
    });

    i++;
  }


// Minerals block
  QGridLayout* mineralRequestLayout = new QGridLayout();


  for (int i = 0; i < 7; i++) {
    QLineEdit* edit = new QLineEdit(QString::number(0), this);
    QLabel* checkLabel = new QLabel(this);
    mineralRequestLayout->addWidget(new QLabel(miniralNames[i], this), i, 0, 1, 1, Qt::AlignLeft);
    mineralRequestLayout->addWidget(edit, i, 1, 1, 1);
    mineralRequestLayout->addWidget(checkLabel, i, 2, 1, 1);

    connect(edit, SIGNAL(textEdited(QString)), this, SLOT(validateOreSelection()));
    mineralEdits_.push_back(edit);
    mineralAvailability_.push_back(checkLabel);
    edit->setAutoFillBackground(false);
  }

  oreResult = new QTextEdit(this);
  oreResult->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  oreResult->setMinimumHeight(50);
  oreResult->setMinimumWidth(150);
  oreResult->setReadOnly(true);

  QGridLayout* mineralsExtrasLayout = new QGridLayout();
  for (int i = 0; i < 7; i++) {
    QLineEdit* extraLabel = new QLineEdit(QString::number(0), this);
    extraLabel->setEnabled(false);
    mineralsExtrasLayout->addWidget(new QLabel(miniralNames[i], this), i, 0);
    mineralsExtrasLayout->addWidget(extraLabel, i, 1);
    mineralExtras_.push_back(extraLabel);
  }

  QVBoxLayout* oreResultLayout = new QVBoxLayout();
  oreResultLayout->addWidget(oreResult);

  QGroupBox* mineralRequestGroup = new QGroupBox("Request", this);
  QGroupBox* oreResultGroup = new QGroupBox("Result", this);
  QGroupBox* mineralsExtrasGroup = new QGroupBox("Extras", this);

  mineralRequestGroup->setLayout(mineralRequestLayout);
  oreResultGroup->setLayout(oreResultLayout);
  mineralsExtrasGroup->setLayout(mineralsExtrasLayout);

  QHBoxLayout* mineralLayout = new QHBoxLayout();
  mineralLayout->addWidget(mineralRequestGroup);
  mineralLayout->addWidget(oreResultGroup);
  mineralLayout->addWidget(mineralsExtrasGroup);
  mineralLayout->setStretch(1, 0);
  mineralLayout->setStretch(0, 1);
  mineralLayout->setStretch(2, 1);

  submit = new QPushButton("Do the math!", this);
  connect(submit, SIGNAL(clicked(bool)), this, SLOT(getCompressedOre()));
  connect(oreReprocessingMode_, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this, SLOT(updateReprocessedMinerals()));
  connect(oreReprocessingMode_, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this, SLOT(validateOreSelection()));

  QVBoxLayout* l = new QVBoxLayout(this);
  l->addWidget(oresViewGroup);
  l->addWidget(skillsGroup);
  l->addLayout(mineralLayout);
  l->addWidget(submit);
}

void OreReprocessingWidget::saveSettings() const
{
  QVector<int> skillLevels;

  skillLevels.push_back(refinerySkillEditor->currentIndex());
  skillLevels.push_back(refineEfficiencySkillEditor->currentIndex());
  skillLevels.push_back(implantEditor->currentIndex());
  skillLevels.push_back(specImplantEditor->currentIndex());

  auto keys = skillEditors_.keys();
  for (int i = 0; i < keys.size(); i++)
    skillLevels.push_back(skillEditors_[keys[i]]->currentIndex());
}

void OreReprocessingWidget::restoreSettings() const
{
}

void OreReprocessingWidget::getCompressedOre()
{
  QVector<QVector<double>> oreTable;

  QVector<ItemRefinery> selected = oreReprocessingMode_->selectedItems();

  for (const ItemRefinery& ir : selected)
    oreTable.push_back(ir.minerals);

  QVector<double> target;
  for (auto edit : mineralEdits_)
    target.push_back(edit->text().toDouble());

  QVector<double> prices (oreTable.size(), 0.);
  for (int i = 0; i < oreTable.size(); i++)
    prices[i] = selected[i].price;

  Simplex<QVector<QVector<double>>, QVector<double>, double> s(oreTable, target, prices, true);
  auto res = s.optimize();

  oreResult->clear();

  if (res.size() != selected.size()) {
    qDebug() << "Smth went wrong";
    return;
  }

  for (int i = 0; i < res.size(); i++) {
    QString str = QString("%1 %2").arg(std::ceil(res[i])).arg(selected[i].name);
    oreResult->append(str);
  }

  double resVal = 0.;
  for (int i = 0; i < res.size(); i++)
    resVal += res[i] * prices[i];

  oreResult->append(QString("\nEstimated price: %1").arg(resVal, 0, 'f', 2));

  QVector<long> result(7, 0);
  for (int i = 0; i < res.size(); i++) {
    for (int j = 0; j < 7; j++)
      result[j] += oreTable[i][j] * std::ceil(res[i]);
  }

  for (int i = 0; i < 7; i++)
    mineralExtras_[i]->setText(PriceInfo::formatPrice(result[i] - target[i], 0, ""));
}

void OreReprocessingWidget::updateReprocessedMinerals()
{
  QVector<ItemRefinery> selection = oreReprocessingMode_->selectedItems();

  selectedMinerals_.fill(false);

  for (const ItemRefinery& s : selection) {
    for (int i = 0; i < s.minerals.size(); i++) {
      if (s.minerals[i] > 0)
        selectedMinerals_[i] = true;
    }
  }
}

void OreReprocessingWidget::validateOreSelection()
{
  QStringList requiredMinerals;

  for (int i = 0; i < 7; i++) {
    QLineEdit* e = mineralEdits_[i];
    if (e->text().toInt() > 0) {
      if (selectedMinerals_[i])
        mineralAvailability_[i]->setText(QChar(0x2714));
      else {
        mineralAvailability_[i]->setText(QChar(0x274C));
        requiredMinerals.append(miniralNames[i]);
      }
    } else
      mineralAvailability_[i]->setText("");
  }

  if (requiredMinerals.isEmpty())
    submit->setText("Do the math");
  else
    submit->setText(QString("Select at least 1 ore contains %1").arg(requiredMinerals.join(", ")));
}

QVector<OreReprocessingWidget::SkillType> OreReprocessingWidget::reprocessingSkills() const
{
  QSqlDatabase db = eve::SDE.getConnection();

  QString query = "SELECT "
                  "  DISTINCT skills.valueInt as skillID, "
                  "  it2.typeName as skillName "
                  "FROM "
                  "  invTypes it "
                  "INNER JOIN invGroups ig ON "
                  "  it.groupID=ig.groupID "
                  "INNER JOIN dgmTypeAttributes skills ON "
                  "  skills.typeID=it.typeID "
                  "INNER JOIN invTypes it2 ON "
                  "  it2.typeID=skills.valueInt "
                  "WHERE "
                  "  ig.categoryID=25 AND "
                  "  it.marketGroupID < 1000 AND "
                  "  it.typeName LIKE 'Compressed %' AND "
                  "  attributeID=790 "
                  "ORDER BY ig.groupID ASC, it.basePrice ASC;";

  QVector<SkillType> skills;
  QSqlQuery q(db);
  if (!q.exec(query)) {
    qWarning() << q.lastError();
    return skills;
  }

  while (q.next()) {
    int skillID = q.value("skillID").toInt();
    QString skillName = q.value("skillName").toString();
    SkillType s {skillID, skillName};

    skills.push_back(s);
  }

  return skills;
}
