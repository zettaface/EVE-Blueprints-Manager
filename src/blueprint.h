#ifndef BLUEPRINT_H
#define BLUEPRINT_H

#include <QVector>
#include <QSet>
#include <QString>
#include <QVariant>
//#include "blueprint.h"

class Blueprint
{
  public:
    Blueprint() = default;
    virtual ~Blueprint() = default;

    virtual QVariant data(int column) const;
    virtual QVariant displayData(int column) const;
    static  QVariant headerData(int column);

    long typeID {-1};
    long prodTypeID {-1};
    int prodGroupID {-1};
    int prodCategoryID {-1};
    int activity {0};
    int prodMetaLevel {-1};
    QSet<int> prodMarketGroups;

    QString typeName;
    qlonglong locationID {0};
    char		ME {0};
    char		TE {0};
    bool		original {false};
    int quantity {0};
    int runs {0};
    int ownerID {-1};

    enum Columns
    {
      Name = 0,
      Activity,
      Runs,
      Quantity,
      MEc,
      TEc,
      Owner,
      LocationIDc,
      MetaLevel,
      ColumnCount,
    };
};

class BlueprintHeader : public Blueprint
{
  public:
    BlueprintHeader() : Blueprint() {}
    BlueprintHeader(const Blueprint& bp);
    QVariant data(int column) const override;
    QVariant displayData(int column) const override;

    void initFrom(const Blueprint& bp);
    void addBlueprint(const Blueprint& bp);

    Blueprint toBlueprint() const;

    bool hasOriginal {false};
    QVector<Blueprint> blueprints;
};

Q_DECLARE_METATYPE(Blueprint)
Q_DECLARE_METATYPE(BlueprintHeader)

bool BlueprintComparator(const Blueprint&, const Blueprint&);

#endif // BLUEPRINT_H
