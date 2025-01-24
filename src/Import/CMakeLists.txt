project(import)

set(SOURCES
    ColumnsPreview.cpp
    ColumnsPreview.h
    DatasetImportTab.cpp
    DatasetImportTab.h
    DatasetsListBrowser.cpp
    DatasetsListBrowser.h
    DatasetVisualization.cpp
    DatasetVisualization.h
    ImportData.cpp
    ImportData.h
    ImportTab.cpp
    ImportTab.h
    SpreadsheetsImportTab.cpp
    SpreadsheetsImportTab.h
)

set(UI
    DatasetsListBrowser.ui
    DatasetVisualization.ui
    SpreadsheetsImportTab.ui
)
source_group("UI Files" FILES ${UI})


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI})

target_link_libraries(${PROJECT_NAME} eible wble QuaZip Qt6::Widgets)
