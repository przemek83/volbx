CODECFORTR = UTF-8
CODECFORSRC = UTF-8

QMAKE_CXXFLAGS += -Wno-unused-local-typedefs

QT += xml testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#Boost
BOOST_ROOT = $$(BOOST_ROOT)
#BOOST_ROOT = D:/Libraries/BOOST/
isEmpty(BOOST_ROOT) {
    message(Environment variable `BOOST_ROOT` not found.)
}
else {
    INCLUDEPATH += $$BOOST_ROOT
}

#QUAZIP
QUAZIP_ROOT = $$(QUAZIP_ROOT)
#QUAZIP_ROOT = D:/Libraries/quazip
isEmpty(QUAZIP_ROOT) {
    message(Environment variable `QUAZIP_ROOT` not found.)
}
else {
    INCLUDEPATH += $${QUAZIP_ROOT}include
    LIBS += -L$${QUAZIP_ROOT}lib -lquazip
}

TARGET = UnitTests
TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle

SOURCES = \
    Common/Configuration.cpp \
    UnitTests/Tests.cpp \
    UnitTests/ConfigurationTest.cpp \
    Common/Constants.cpp \
    UnitTests/ConstantsTest.cpp \
    Shared/Logger.cpp \
    DataSets/DatasetInner.cpp \
    DataSets/DatasetDefinitionXlsx.cpp \
    DataSets/DatasetDefinitionSpreadsheet.cpp \
    DataSets/DatasetDefinitionOds.cpp \
    DataSets/DatasetDefinitionInner.cpp \
    DataSets/DatasetDefinition.cpp \
    DataSets/Dataset.cpp \
    Common/ProgressBar.cpp \
    DataSets/DatasetSpreadsheet.cpp \
    ModelsAndViews/TableModel.cpp \
    ModelsAndViews/FilteringProxyModel.cpp \
    Common/ExportData.cpp \
    UnitTests/SpreadsheetsTest.cpp \
    UnitTests/InnerTests.cpp \
    UnitTests/Common.cpp

HEADERS += \
    Common/Configuration.h \
    UnitTests/ConfigurationTest.h \
    Common/Constants.h \
    UnitTests/ConstantsTest.h \
    Shared/Logger.h \
    Shared/Debugging.h \
    DataSets/DatasetInner.h \
    DataSets/DatasetDefinitionXlsx.h \
    DataSets/DatasetDefinitionSpreadsheet.h \
    DataSets/DatasetDefinitionOds.h \
    DataSets/DatasetDefinitionInner.h \
    DataSets/DatasetDefinition.h \
    DataSets/Dataset.h \
    Common/ProgressBar.h \
    DataSets/DatasetSpreadsheet.h \
    ModelsAndViews/TableModel.h \
    ModelsAndViews/FilteringProxyModel.h \
    Common/ExportData.h \
    UnitTests/SpreadsheetsTest.h \
    UnitTests/InnerTests.h \
    UnitTests/Common.h

#TEMPLATE = app

























