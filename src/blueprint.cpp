#include "blueprint.h"

#include <QAbstractItemModel>
#include <QDebug>

#include <tuple>
#include <algorithm>

#include "eve/api.h"
#include "eve/staticdata.h"

bool BlueprintComparator(const Blueprint& b1, const Blueprint& b2)
{
  return std::tie(b1.typeName,
                  b1.original,
                  b1.quantity,
                  b1.runs,
                  b1.ME,
                  b1.TE)
         <
         std::tie(b2.typeName,
                  b2.original,
                  b2.quantity,
                  b2.runs,
                  b2.ME,
                  b2.TE);
}

QVariant Blueprint::data(int column) const
{
  switch(column) {
    case Name:
      return typeName;
    case Activity:
      return activity;
    case Runs:
      return runs;
    case Quantity:
      return quantity;
    case MEc:
      return ME;
    case TEc:
      return TE;
    case LocationIDc:
      return locationID;
    case Owner:
      return ownerID;
    case MetaLevel:
      return prodMetaLevel;
    default:
      qWarning() << "Wrong column";
      return QVariant();
  }
}

QVariant Blueprint::displayData(int column) const
{
  const QMap<int, QString> activities {
    {0, ""},
    {1, "Manufacturing"},
    {2, "Researching Technology"},
    {3, "Researching Time Efficiency"},
    {4, "Researching Material Efficiency"},
    {5, "Copying"},
    {6, "Duplicating"},
    {7, "Reverse Engineering"},
    {8, "Invention"}
  };

  switch(column) {
    case Name:
      return typeName;
    case Activity:
      return activities[activity];
    case Runs:
      if (runs < 0)
        return QChar(0x221E);
      else
        return runs;
    case Quantity:
      return quantity;
    case MEc:
      if (ME < 0)
        return QVariant();
      else
        return ME;
    case TEc:
      if (TE < 0)
        return QVariant();
      else
        return TE;
    case LocationIDc:
      return eve::SDE.locationName(locationID);
    case Owner:
      if (ownerID < 0)
        return "-";

      if (eve::API.keys()->keyByKeyID(ownerID) == nullptr)
        return "Invalid API Key";

      return eve::API.keys()->keyByKeyID(ownerID)->name();
    case MetaLevel:
      return prodMetaLevel;
    default:
      qWarning("Wrong column");
      return QVariant();
  }
}

QVariant Blueprint::headerData(int column)
{
  switch(column) {
    case Name:
      return "Name";
    case Activity:
      return "";
    case Runs:
      return "Runs";
    case Quantity:
      return "Quantity";
    case MEc:
      return "ME";
    case TEc:
      return "TE";
    case LocationIDc:
      return "Location";
    case Owner:
      return "Owner";
    case MetaLevel:
      return "Prod. meta lvl";
    default:
      return "Unknown column";
  }
}


BlueprintHeader::BlueprintHeader(const Blueprint& bp)
{
  initFrom(bp);
}

QVariant BlueprintHeader::data(int column) const
{
  return Blueprint::data(column);
}

QVariant BlueprintHeader::displayData(int column) const
{
  switch(column) {
    case Runs:
      if (runs < 0)
        return QChar(0x221E);
      if (runs > 0 && hasOriginal)
        return QString("%1+%2").arg(runs).arg(QChar(0x221E));
      else
        return runs;
    default:
      return Blueprint::displayData(column);
  }
}

void BlueprintHeader::initFrom(const Blueprint& bp)
{
  typeName         = bp.typeName;
  prodCategoryID   = bp.prodCategoryID;
  prodGroupID      = bp.prodGroupID;
  prodMetaLevel    = bp.prodMetaLevel;
  typeID           = bp.typeID;
  prodTypeID       = bp.prodTypeID;
  TE               = bp.TE;
  ME               = bp.ME;
  locationID       = bp.locationID;
  prodMarketGroups = bp.prodMarketGroups;
  ownerID          = bp.ownerID;
}

void BlueprintHeader::addBlueprint(const Blueprint& bp)
{
  if (bp.original)
    hasOriginal = true;
  //else
  if (runs <= 0 && bp.original)
    runs = -1;
  else if (runs <= 0 && !bp.original)
    runs = bp.runs * bp.quantity;
  else if (runs > 0 && !bp.original)
    runs += bp.runs * bp.quantity;

  if (locationID != 0 && locationID != bp.locationID)
    locationID = 0;

  if (ME >= 0 && bp.ME != ME)
    ME = -1;

  if (TE >= 0 && bp.TE != TE)
    TE = -1;

  if (ownerID >= 0 && bp.ownerID != ownerID)
    ownerID = -1;

  quantity += bp.quantity;

  blueprints.push_back(bp);
}

Blueprint BlueprintHeader::toBlueprint() const
{
  Blueprint bp(*this);
  return bp;
}
