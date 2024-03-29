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

# QWT
if(DEFINED ENV{QWT_ROOT})
    message(STATUS "QWT_ROOT environment variable defined, using QWT_ROOT paths.")
else()
    configure_file(CMakeLists.txt.qwt qwt/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qwt)
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qwt)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/qwt/install)
    file(COPY ${CMAKE_BINARY_DIR}/qwt/qwt/src/qwt-build/lib DESTINATION ${CMAKE_BINARY_DIR}/qwt/install)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/qwt/install/include)
    file(GLOB QWT_HEADERS "${CMAKE_BINARY_DIR}/qwt/qwt/src/qwt/src/*.h" )
    file(COPY ${QWT_HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/qwt/install/include)
    set(ENV{QWT_ROOT} ${CMAKE_BINARY_DIR}/qwt/install/)
endif()

set(THIRD_PARTY_LIBS wble qwtble eible zlib quazip)
foreach(library ${THIRD_PARTY_LIBS})
    configure_file(CMakeLists.txt.${library} ${library}/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} .
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${library})
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${library} )
endforeach(library)

include_directories($ENV{QWT_ROOT}/include)

foreach(file_i ${THIRD_PARTY_LIBS})
    include_directories(${CMAKE_BINARY_DIR}/${file_i}/install/include)
endforeach(file_i)

set(ZLIB_ROOT ${CMAKE_BINARY_DIR}/zlib/install)
find_package(ZLIB REQUIRED)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/quazip/install/")
find_package(QuaZip-Qt6 CONFIG REQUIRED)

link_directories($ENV{QWT_ROOT}/lib)
link_directories(${CMAKE_BINARY_DIR}/zlib/install/bin)
link_directories(${CMAKE_BINARY_DIR}/quazip/install/lib)
link_directories(${CMAKE_BINARY_DIR}/wble/install/lib)
link_directories(${CMAKE_BINARY_DIR}/qwtble/install/lib)
link_directories(${CMAKE_BINARY_DIR}/eible/install/lib)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(GUI)
include_directories(Import)
include_directories(Shared)
include_directories(Common)
include_directories(Datasets)
include_directories(ModelsAndViews)
include_directories(Export)

add_subdirectory(Shared)
add_subdirectory(Common)
add_subdirectory(Datasets)
add_subdirectory(ModelsAndViews)
add_subdirectory(Export)

set(SOURCES
    Volbx/main.cpp
    Volbx/Volbx.ico
    Volbx/VolbxProperties.rc
    GUI/About.cpp
    GUI/CheckUpdates.cpp
    GUI/DockTitleBar.cpp
    GUI/Dock.cpp
    GUI/Export.cpp
    GUI/FiltersDock.cpp
    GUI/Tab.cpp
    GUI/TabWidget.cpp
    GUI/PlotDock.cpp
    GUI/SaveDatasetAs.cpp
    GUI/TabBar.cpp
    GUI/DataViewDock.cpp
    GUI/VolbxMain.cpp
    Import/ColumnsPreview.cpp
    Import/DatasetVisualization.cpp
    Import/DatasetImportTab.cpp
    Import/DatasetsListBrowser.cpp
    Import/ImportData.cpp
    Import/ImportTab.cpp
    Import/SpreadsheetsImportTab.cpp
)
qt_add_resources(SOURCES Resources/Resources.qrc)

set(HEADERS
    Volbx/VolbxProperties.h
    GUI/About.h
    GUI/CheckUpdates.h
    GUI/DockTitleBar.h
    GUI/Dock.h
    GUI/Export.h
    GUI/FiltersDock.h
    GUI/Tab.h
    GUI/TabWidget.h
    GUI/PlotDock.h
    GUI/SaveDatasetAs.h
    GUI/TabBar.h
    GUI/DataViewDock.h
    GUI/VolbxMain.h
    Import/ColumnsPreview.h
    Import/DatasetVisualization.h
    Import/DatasetImportTab.h
    Import/DatasetsListBrowser.h
    Import/ImportData.h
    Import/ImportTab.h
    Import/SpreadsheetsImportTab.h
)

set(UI
    GUI/About.ui
    GUI/CheckUpdates.ui
    GUI/DockTitleBar.ui
    GUI/Export.ui
    GUI/SaveDatasetAs.ui
    GUI/VolbxMain.ui
    Import/SpreadsheetsImportTab.ui
    Import/DatasetsListBrowser.ui
    Import/DatasetVisualization.ui
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

add_subdirectory(Update)

enable_testing()
add_subdirectory(Tests)
