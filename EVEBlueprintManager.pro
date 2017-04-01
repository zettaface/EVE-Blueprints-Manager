QT       += core network xml gui sql concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = app

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS_RELEASE += -static -static-libgcc

LIBS += -L"C:/libs/yamlf" -lyaml-cpp
INCLUDEPATH += "C:/libs/yamlf/include/"

win32:RC_ICONS = images/Icon_32.ico
ICON = images/Icon_64.icns

VERSION = 0.1.1
DEFINES += APP_VERSION_STR=\\\"$$VERSION\\\"

SOURCES += \
    src/main.cpp \
    src/assettree.cpp \
    src/mainwindow.cpp \
    src/blueprintlist.cpp \
    src/apikeyinsertdialog.cpp \
    src/apikeymanagmentdialog.cpp \
    src/blueprintlistcolorizeproxymodel.cpp \
    src/bpcolordialog.cpp \
    src/categoryfilterproxymodel.cpp \
    src/bpcolorlistitemdelegate.cpp \
    src/marketgroupsmodel.cpp \
    src/hierarchicalcombobox.cpp \
    src/blueprintcolorizer.cpp \
    src/blueprint.cpp \
    src/bpcolorsview.cpp \
    src/marketidfiltermodel.cpp \
    src/bpgroupsmodel.cpp \
    src/bpcategoriesmodel.cpp \
    src/productionmodel.cpp \
    src/productiontype.cpp \
    src/priceinfo.cpp \
    src/blueprintpage.cpp \
    src/productionmodelitemdelegate.cpp \
    src/blueprintsfilterproxy.cpp \
    src/blueprintsfilterwidget.cpp \
    src/blueprintssortproxy.cpp \
    src/blueprintsdecorationproxy.cpp \
    src/oretablemodel.cpp \
    src/orereprocessingwidget.cpp \
    src/oreselectionwidget.cpp \
    src/bpview.cpp \
    src/blueprintsaggregationproxy.cpp \
    src/blueprintsfilterboxwidget.cpp \
    src/bpfilterfactory.cpp \
    src/blueprintsemptygroupsfilter.cpp \
    src/industryactivitytypemodel.cpp \
    src/blueprintscolumnfilter.cpp \
    src/filters/bpcategorygroupfilter.cpp \
    src/filters/bpfilter.cpp \
    src/filters/bpmarketgroupfilter.cpp \
    src/filters/bpcomplexfilter.cpp \
    src/filters/bpstringfilter.cpp \
    src/filters/bpnumericfilter.cpp \
    src/filters/bpisheaderfilter.cpp \
    src/filters/bpactivityfilter.cpp \
    src/bpcategorygroupfilterproxy.cpp \
    src/bpcolorsmodel.cpp \
    src/filterEditors/categorygroupfiltereditor.cpp \
    src/filterEditors/bpfiltereditor.cpp \
    src/filterEditors/marketgroupfiltereditor.cpp \
    src/filterEditors/singlelineeditor.cpp \
    src/filterEditors/combolineeditor.cpp \
    src/filterEditors/complexeditor.cpp \
    src/filterEditors/activityfiltereditor.cpp \
    src/filterEditors/isheaderfiltereditor.cpp \
    src/blueprintswidget.cpp \
    src/tabwidget.cpp \
    src/eve/api.cpp \
    src/eve/apikey.cpp \
    src/eve/apikeylist.cpp \
    src/eve/staticdata.cpp \
    src/eve/parser/assetxmlparser.cpp \
    src/eve/parser/blueprintsxmlparser.cpp \
    src/eve/parser/keyinfoxmlparser.cpp \
    src/eve/parser/locationsxmlparser.cpp \
    src/eve/parser/industryjobsxmlparser.cpp \
    src/eve/parser/conquerablestationxmlparser.cpp \
    src/eve/downloadmanager.cpp \
    src/eve/parser/parser.cpp \
    src/eve/parser/inventorytypeimageparser.cpp \
    src/eve/request.cpp \
    src/eve/apirequest.cpp \
    src/eve/crestapirequest.cpp \
    src/eve/crest/market/orders.cpp \
    src/eve/crest.cpp \
    src/eve/crest/market/history.cpp \
    src/eve/parser/marketordersparser.cpp \
    src/eve/imageserverrequest.cpp \
    src/eve/keystatusparsererrormodel.cpp \
    src/eve/removeexpiredkeysdialog.cpp \
    src/eve/keysrequest.cpp \
    src/eve/parser/apikeyupdater.cpp \
    src/eve/parser/xmlparser.cpp \
    src/eve/removeduplicatekeysdialog.cpp \
    src/eve/downloadstatusdialog.cpp \
    src/eve/updateapikeysdialog.cpp

HEADERS += \
    src/assettree.h \
    src/mainwindow.h \
    src/blueprintlist.h \
    src/apikeyinsertdialog.h \
    src/apikeymanagmentdialog.h \
    src/blueprintlistcolorizeproxymodel.h \
    src/bpcolordialog.h \
    src/categoryfilterproxymodel.h \
    src/bpcolorlistitemdelegate.h \
    src/marketgroupsmodel.h \
    src/hierarchicalcombobox.h \
    src/blueprintcolorizer.h \
    src/blueprint.h \
    src/bpcolorsview.h \
    src/marketidfiltermodel.h \
    src/bpgroupsmodel.h \
    src/bpcategoriesmodel.h \
    src/productionmodel.h \
    src/productiontype.h \
    src/priceinfo.h \
    src/blueprintpage.h \
    src/productionmodelitemdelegate.h \
    src/blueprintsfilterproxy.h \
    src/blueprintsfilterwidget.h \
    src/blueprintssortproxy.h \
    src/blueprintsdecorationproxy.h \
    src/oretablemodel.h \
    src/orereprocessingwidget.h \
    src/simplex.h \
    src/oreselectionwidget.h \
    src/bpview.h \
    src/blueprintsaggregationproxy.h \
    src/blueprintsfilterboxwidget.h \
    src/bpfilterfactory.h \
    src/abstractfactory.h \
    src/blueprintsemptygroupsfilter.h \
    src/industryactivitytypemodel.h \
    src/blueprintscolumnfilter.h \
    src/filters/bpcategorygroupfilter.h \
    src/filters/bpfilter.h \
    src/filters/bpmarketgroupfilter.h \
    src/filters/bpcomplexfilter.h \
    src/filters/bpstringfilter.h \
    src/filters/bpnumericfilter.h \
    src/filters/bpisheaderfilter.h \
    src/filters/bpactivityfilter.h \
    src/filters/blueprintfilters.h \
    src/bpcategorygroupfilterproxy.h \
    src/bpcolorsmodel.h \
    src/filterEditors/categorygroupfiltereditor.h \
    src/filterEditors/bpfiltereditor.h \
    src/filterEditors/marketgroupfiltereditor.h \
    src/filterEditors/singlelineeditor.h \
    src/filterEditors/combolineeditor.h \
    src/filterEditors/complexeditor.h \
    src/filterEditors/activityfiltereditor.h \
    src/filterEditors/isheaderfiltereditor.h \
    src/filterEditors/filtereditors.h \
    src/blueprintswidget.h \
    src/tabwidget.h \
    src/eve/api.h \
    src/eve/apikey.h \
    src/eve/apikeylist.h \
    src/eve/staticdata.h \
    src/eve/parser/assetxmlparser.h \
    src/eve/parser/blueprintsxmlparser.h \
    src/eve/parser/keyinfoxmlparser.h \
    src/eve/parser/locationsxmlparser.h \
    src/eve/parser/industryjobsxmlparser.h \
    src/eve/parser/conquerablestationxmlparser.h \
    src/eve/downloadmanager.h \
    src/eve/parser/parser.h \
    src/eve/parser/parsers.h \
    src/eve/parser/inventorytypeimageparser.h \
    src/eve/request.h \
    src/eve/apirequest.h \
    src/eve/crestapirequest.h \
    src/eve/crest.h \
    src/eve/crest/inventory.h \
    src/eve/crest/crestapiendpoint.h \
    src/eve/crest/market.h \
    src/eve/crest/market/history.h \
    src/eve/crest/market/orders.h \
    src/eve/crest/inventory/type.h \
    src/eve/crest/inventory/groups.h \
    src/eve/parser/marketordersparser.h \
    src/eve/imageserverrequest.h \
    src/eve/keystatusparsererrormodel.h \
    src/eve/removeexpiredkeysdialog.h \
    src/eve/keysrequest.h \
    src/eve/parser/apikeyupdater.h \
    src/eve/parser/xmlparser.h \
    src/eve/removeduplicatekeysdialog.h \
    src/eve/downloadstatusdialog.h \
    src/eve/updateapikeysdialog.h

RESOURCES += \
    resources.qrc

