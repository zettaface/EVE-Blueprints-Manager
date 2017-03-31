#include "blueprintpage.h"

#include <QLineEdit>
#include <QLabel>
#include <QIntValidator>
#include <QSpinBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QFrame>
#include <QDebug>

#include "eve/staticdata.h"
#include "eve/api.h"

#include "productionmodelitemdelegate.h"

BlueprintPage::BlueprintPage(const Blueprint& bp, QWidget* parent) :
  QWidget(parent)
{
  model_ = new ProductionModel(bp, this);

  QString nameString = bp.displayData(Blueprint::Name).toString();
  QString formattedNameString = QString("<b>%1</b>").arg(nameString);

  QLabel* nameLabel = new QLabel(formattedNameString, this);
  nameLabel->setMinimumWidth(200);

  QLineEdit* meEdit   = new QLineEdit(QString::number(bp.ME), this);
  QLineEdit* teEdit   = new QLineEdit(QString::number(bp.TE), this);

  connect(meEdit, &QLineEdit::textEdited, [this](QString text) {
    if (!text.isEmpty())
      model_->product()->setME(text.toInt());
  });

  QLineEdit* runsEdit = new QLineEdit("1", this);

  marketPriceEdit = new QLineEdit("0.00 ISK", this);
  buildPriceEdit   = new QLineEdit("0.00 ISK", this);

  marketPriceEdit->setReadOnly(true);
  buildPriceEdit->setReadOnly(true);

  nameLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
  runsEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  marketPriceEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  buildPriceEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

  marketPriceEdit->setAlignment(Qt::AlignCenter);
  buildPriceEdit->setAlignment(Qt::AlignCenter);

  meEdit->setValidator(new QIntValidator(0, 10, this));
  teEdit->setValidator(new QIntValidator(0, 20, this));
  runsEdit->setValidator(new QIntValidator(0, 1000, this));

  meEdit->setMaximumWidth(30);
  teEdit->setMaximumWidth(30);
  runsEdit->setMinimumWidth(50);
  runsEdit->setMaximumWidth(50);

  QLabel* meLabel   = new QLabel("ME:",   this);
  QLabel* teLabel   = new QLabel("TE:",   this);
  QLabel* runsLabel = new QLabel("Runs:", this);

  componentsView_ = new QTreeView(this);

  componentsView_->setModel(model_);
  componentsView_->setAlternatingRowColors(true);
  componentsView_->setRootIndex(model_->index(0, 0));
  componentsView_->setColumnWidth(ProductionModel::NameColumn, 300);
  componentsView_->setColumnWidth(ProductionModel::MEColumn, 35);
  componentsView_->setColumnWidth(ProductionModel::TEColumn, 35);
  componentsView_->setAlternatingRowColors(true);
  componentsView_->setItemDelegate(new ProductionModelItemDelegate);

  QGridLayout* lt = new QGridLayout(this);

  lt->setSpacing(1);
  QPixmap image = eve::API.typeIcon(bp.typeID).scaledToHeight(64);
  QLabel* iconLabel = new QLabel(this);
  iconLabel->setPixmap(image);
  QVBoxLayout* iconLayout = new QVBoxLayout;
  iconLayout->addWidget(iconLabel);
  QFrame* iconWidget = new QFrame(this);
  iconWidget->setLayout(iconLayout);
  iconWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
  iconWidget->setLineWidth(1);
  iconWidget->setVisible(false);


  lt->addWidget(iconWidget, 0, 0, 2, 1);
  lt->addWidget(nameLabel, 0, 1);
  lt->addWidget(new QLabel("Production time placeholder"), 1, 1);
  lt->addWidget(meLabel,   0, 2);
  lt->addWidget(meEdit,    1, 2);
  lt->addWidget(teLabel,   0, 3);
  lt->addWidget(teEdit,    1, 3);
  lt->addWidget(runsLabel, 0, 4);
  lt->addWidget(runsEdit,  1, 4);
  lt->addWidget(new QLabel("Market Price:"), 0, 5);
  lt->addWidget(new QLabel("Build Price:"),  0, 6);
  lt->addWidget(marketPriceEdit, 1, 5);
  lt->addWidget(buildPriceEdit,  1, 6);

  lt->addItem(new QSpacerItem(0, 10, QSizePolicy::Minimum), 2, 0, 1, 7);

  lt->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, 7, 2, 1);
  lt->addWidget(componentsView_, 3, 0, 1, 8);

  connect(model_, SIGNAL(priceChanged(double)), this, SLOT(onPriceChange(double)));

  buildPriceAnimation_ = new QVariantAnimation(this);
  connect(buildPriceAnimation_, SIGNAL(valueChanged(QVariant)), this, SLOT(updatePriceBox(QVariant)));

  onPriceChange(model_->product()->data(ProductionType::BuildPrice).toDouble());
}

void BlueprintPage::onPriceChange(double newPrice)
{
  buildPriceAnimation_->setStartValue(currentPrice_);
  buildPriceAnimation_->setEndValue(newPrice);
  buildPriceAnimation_->setDuration(400);
  buildPriceAnimation_->start();

  marketPriceEdit->setText(model_->product()->displayData(ProductionType::MarketPrice).toString());
}

void BlueprintPage::updatePriceBox(QVariant price)
{
  currentPrice_ = price.toDouble();

  buildPriceEdit->setText(ProductionType::formatPrice(currentPrice_));
}
