CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8

QMAKE_CXXFLAGS += -Wno-unused-local-typedefs

#Resolve macros.
#QMAKE_CXXFLAGS += -E

#TRANSLATIONS = lang_us.ts

RC_FILE = Volbx/VolbxProperties.rc

#DEFINES += QT_NO_CAST_FROM_ASCII

#QT
QT += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#System variables BOOST_ROOT, QWT_ROOT, QUAZIP_ROOT should point to correct
#install dirs. Instead of env variable you can also add your paths directly
#using commented lines.

#QWT
QWT_ROOT = $$(QWT_ROOT)
#QWT_ROOT = D:/Libraries/qwt
isEmpty(QWT_ROOT) {
    message(Environment variable `QWT_ROOT` not found.)
}
else {
    INCLUDEPATH += $${QWT_ROOT}include
    LIBS += -L$${QWT_ROOT}lib
    CONFIG(debug, debug|release) {
        LIBS += -lqwtd
    } else {
        LIBS += -lqwt
    }
}

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

TARGET = Volbx
TEMPLATE = app

SOURCES += \ 
    GUI/DockWidget.cpp \
    GUI/DoubleSlider.cpp \
    ModelsAndViews/FilteringProxyModel.cpp \
    ModelsAndViews/NumericDelegate.cpp \
    ModelsAndViews/DateDelegate.cpp \
    DataProvider/PlotDataProvider.cpp \
    Charts/Zoomer.cpp \
    Charts/PlotBase.cpp \
    Common/Quantiles.cpp \
    GUI/VolbxMain.cpp \
    Volbx/main.cpp \
    Charts/Picker.cpp \
    GUI/About.cpp \
    GUI/DockTitleBar.cpp \
    GUI/Export.cpp \
    GUI/ViewDockWidget.cpp \
    GUI/PlotDockWidget.cpp \
    Common/ExportData.cpp \
    ModelsAndViews/TableModel.cpp \
    Common/Constants.cpp \
    Datasets/Dataset.cpp \
    Datasets/DatasetDefinition.cpp \
    GUI/SaveDatasetAs.cpp \
    Common/ProgressBar.cpp \
    DataProvider/TransactionData.cpp \
    Datasets/DatasetDefinitionInner.cpp \
    Datasets/DatasetDefinitionXlsx.cpp \
    Datasets/DatasetDefinitionOds.cpp \
    Datasets/DatasetInner.cpp \
    Datasets/DatasetDefinitionSpreadsheet.cpp \
    GUI/TabBar.cpp \
    GUI/MainTabWidget.cpp \
    ModelsAndViews/DataView.cpp \
    GUI/CheckUpdatesDialog.cpp \
    Common/Configuration.cpp \
    Shared/Networking.cpp \
    Shared/Logger.cpp \
    Shared/Application.cpp \
    Charts/QuantilesPlot.cpp \
    Charts/GroupPlot.cpp \
    GUI/GroupPlotGui.cpp \
    Charts/BasicDataPlot.cpp \
    Charts/CustomMarker.cpp \
    Charts/NotchedMarker.cpp \
    Charts/StringsScaleDraw.cpp \
    Filters/FiltersDock.cpp \
    Filters/FilterNumbers.cpp \
    Filters/FilterNames.cpp \
    Filters/FilterDates.cpp \
    Filters/Filter.cpp \
    Datasets/Datasetspreadsheet.cpp \
    Common/ExportImage.cpp \
    Common/PlotData.cpp \
    GUI/MainTab.cpp \
    Charts/HistogramPlot.cpp \
    GUI/HistogramPlotGui.cpp \
    Import/ImportData.cpp \
    Import/DatasetsListBrowser.cpp \
    Import/DatasetImportTab.cpp \
    Import/DatasetDefinitionVisualization.cpp \
    Import/ColumnsPreview.cpp \
    Import/ImportTab.cpp \
    Import/SpreadsheetsImportTab.cpp

HEADERS  += \ 
    GUI/DockWidget.h \
    ModelsAndViews/DataView.h \
    GUI/DoubleSlider.h \
    ModelsAndViews/FilteringProxyModel.h \
    ModelsAndViews/NumericDelegate.h \
    ModelsAndViews/DateDelegate.h \
    DataProvider/PlotDataProvider.h \
    Charts/Zoomer.h \
    Charts/PlotBase.h \
    Common/Quantiles.h \
    GUI/VolbxMain.h \
    Charts/Picker.h \
    GUI/About.h \
    GUI/DockTitleBar.h \
    GUI/Export.h \
    GUI/ViewDockWidget.h \
    GUI/PlotDockWidget.h \
    Common/ExportData.h \
    ModelsAndViews/TableModel.h \
    Common/Formats.h \
    Common/Constants.h \
    Common/SpecialColumns.h \
    Datasets/Dataset.h \
    Datasets/DatasetDefinition.h \
    GUI/SaveDatasetAs.h \
    Common/ProgressBar.h \
    DataProvider/TransactionData.h \
    Datasets/DatasetDefinitionInner.h \
    Datasets/DatasetDefinitionXlsx.h \
    Datasets/DatasetDefinitionOds.h \
    Datasets/DatasetInner.h \
    Datasets/DatasetDefinitionSpreadsheet.h \
    GUI/TabBar.h \
    GUI/MainTabWidget.h \
    GUI/CheckUpdatesDialog.h \
    Common/Configuration.h \
    Shared/Networking.h \
    Shared/Logger.h \
    Shared/Debugging.h \
    Shared/CommonProperties.h \
    Shared/Application.h \
    Charts/QuantilesPlot.h \
    Charts/GroupPlot.h \
    GUI/GroupPlotGui.h \
    Charts/BasicDataPlot.h \
    Charts/CustomMarker.h \
    Charts/NotchedMarker.h \
    Charts/StringsScaleDraw.h \
    Filters/FiltersDock.h \
    Filters/FilterNumbers.h \
    Filters/FilterNames.h \
    Filters/FilterDates.h \
    Filters/Filter.h \
    Datasets/Datasetspreadsheet.h \
    Common/ExportImage.h \
    Common/PlotData.h \
    GUI/MainTab.h \
    Charts/HistogramPlot.h \
    GUI/HistogramPlotGui.h \
    Import/ImportData.h \
    Import/DatasetsListBrowser.h \
    Import/DatasetImportTab.h \
    Import/DatasetDefinitionVisualization.h \
    Import/ColumnsPreview.h \
    Import/ImportTab.h \
    Import/SpreadsheetsImportTab.h

FORMS    += \
    GUI/About.ui \
    GUI/DockTitleBar.ui \
    GUI/Export.ui \
    GUI/SaveDatasetAs.ui \
    GUI/VolbxMain.ui \
    GUI/CheckUpdatesDialog.ui \
    Filters/FiltersDock.ui \
    Filters/FilterNumbers.ui \
    Filters/FilterNames.ui \
    Filters/FilterDates.ui \
    GUI/GroupPlotGui.ui \
    GUI/HistogramPlotGui.ui \
    Import/ImportData.ui \
    Import/DatasetsListBrowser.ui \
    Import/DatasetDefinitionVisualization.ui \
    Import/SpreadsheetsImportTab.ui

RESOURCES += \
    Resources/Resources.qrc
