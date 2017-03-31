#ifndef EVESTATICDATA_H
#define EVESTATICDATA_H

#include <QHash>
#include <QSqlDatabase>
#include <QAbstractListModel>

#include "../productiontype.h"
#include "../marketgroupsmodel.h"
#include "../bpgroupsmodel.h"
#include "../bpcategoriesmodel.h"
#include "../industryactivitytypemodel.h"
//class MarketGroupsModel;

struct invType
{
  qlonglong ID;
  int groupID;
  int categoryID;
  int marketGroup;
  int metaGroupID;
  int metaLevel;
  double volume;
  QString typeName;
  QString iconFile;

};

struct RefineInfo
{
  QVector<QPair<invType, int>> materials;
};

namespace eve {

class StaticData
{
  public:
    using categoryData = QPair<int, QString>;
    StaticData();
    ~StaticData();

    void init();

    QString typeName(int typeID);
    QString locationName(qlonglong locationID);
    QString categoryName(int ID);
    QString groupName(int ID);
    QString marketGroupName(int marketGroupID) const;
    void initializeCategoriesModel();
    void initializeGroupsModel();
    invType typeInfo(qlonglong ID);
    RefineInfo refineInfo(const invType& type);
    QPixmap icon(const invType& type);

    QList<int> groupIDs() const;

    BPGroupsModel* bpGroupModel();
    BPCategoriesModel* bpCategoriesModel();
    MarketGroupsModel* marketGroupsModel();
    IndustryActivityTypeModel* industryActivityModel();

    static QSqlDatabase getConnection();
    static QString sdeFileLocation() { return dbLocation; }

    int productTypeIDbyBPTypeID(int typeID);
    void loadTypeName(qlonglong typeID);
    void loadLocationName(qlonglong locationID);
    void initProductionType(ProductionType*);

  private:

    QHash<qlonglong, invType> typeNames;
    QHash<qlonglong, QString> locationNames;
    QVector<invGroup> bpgroups;

    BPCategoriesModel* bpCategories_ {nullptr};
    BPGroupsModel* bpGroups_ {nullptr};
    MarketGroupsModel* marketGroups {nullptr};
    IndustryActivityTypeModel* industryActivities {nullptr};

    static QString dbLocation;
};

extern StaticData SDE;

}

#endif // EVESTATICDATA_H
