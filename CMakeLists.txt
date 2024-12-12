cmake_minimum_required(VERSION 3.16)

project(volbx
        LANGUAGES CXX
        VERSION 2.1.0
        DESCRIPTION "Graphical tool for data manipulation"
        HOMEPAGE_URL "https://github.com/przemek83/volbx")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Network Xml Test)
qt_standard_project_setup()

include(QwtbleConfig.cmake)
include(WbleConfig.cmake)
include(EibleConfig.cmake)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(src/GUI)
include_directories(src/Import)
include_directories(src/Shared)
include_directories(src/Common)
include_directories(src/Datasets)
include_directories(src/ModelsAndViews)
include_directories(src/Export)

add_subdirectory(src/Shared)
add_subdirectory(src/Common)
add_subdirectory(src/Datasets)
add_subdirectory(src/ModelsAndViews)
add_subdirectory(src/Export)

set(SOURCES
    src/Volbx/main.cpp
    src/Volbx/Volbx.ico
    src/Volbx/VolbxProperties.rc
    src/GUI/About.cpp
    src/GUI/CheckUpdates.cpp
    src/GUI/DockTitleBar.cpp
    src/GUI/Dock.cpp
    src/GUI/Export.cpp
    src/GUI/FiltersDock.cpp
    src/GUI/Tab.cpp
    src/GUI/TabWidget.cpp
    src/GUI/PlotDock.cpp
    src/GUI/SaveDatasetAs.cpp
    src/GUI/TabBar.cpp
    src/GUI/DataViewDock.cpp
    src/GUI/VolbxMain.cpp
    src/Import/ColumnsPreview.cpp
    src/Import/DatasetVisualization.cpp
    src/Import/DatasetImportTab.cpp
    src/Import/DatasetsListBrowser.cpp
    src/Import/ImportData.cpp
    src/Import/ImportTab.cpp
    src/Import/SpreadsheetsImportTab.cpp
)
qt_add_resources(SOURCES Resources/Resources.qrc)

set(HEADERS
    src/Volbx/VolbxProperties.h
    src/GUI/About.h
    src/GUI/CheckUpdates.h
    src/GUI/DockTitleBar.h
    src/GUI/Dock.h
    src/GUI/Export.h
    src/GUI/FiltersDock.h
    src/GUI/Tab.h
    src/GUI/TabWidget.h
    src/GUI/PlotDock.h
    src/GUI/SaveDatasetAs.h
    src/GUI/TabBar.h
    src/GUI/DataViewDock.h
    src/GUI/VolbxMain.h
    src/Import/ColumnsPreview.h
    src/Import/DatasetVisualization.h
    src/Import/DatasetImportTab.h
    src/Import/DatasetsListBrowser.h
    src/Import/ImportData.h
    src/Import/ImportTab.h
    src/Import/SpreadsheetsImportTab.h
)

set(UI
    src/GUI/About.ui
    src/GUI/CheckUpdates.ui
    src/GUI/DockTitleBar.ui
    src/GUI/Export.ui
    src/GUI/SaveDatasetAs.ui
    src/GUI/VolbxMain.ui
    src/Import/SpreadsheetsImportTab.ui
    src/Import/DatasetsListBrowser.ui
    src/Import/DatasetVisualization.ui
)
source_group("UI Files" FILES ${UI})

# remove background console window.
if(WIN32)
    set(GUI_TYPE WIN32)
endif(WIN32)
if(APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
endif(APPLE)

add_executable(${PROJECT_NAME} ${GUI_TYPE} ${SOURCES} ${HEADERS} ${UI})

target_link_libraries(${PROJECT_NAME} Threads::Threads shared common datasets modelsAndViews export wble qwtble eible Qt6::Core Qt6::Network Qt6::Widgets Qt6::Gui Qt6::Xml)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} optimized qwt debug qwtd)
else()
    target_link_libraries(${PROJECT_NAME} qwt)
endif()

add_subdirectory(src/Update)

enable_testing()
add_subdirectory(Tests)
