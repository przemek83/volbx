cmake_minimum_required(VERSION 3.16)

project(volbx
        LANGUAGES CXX
        VERSION 2.1.0
        DESCRIPTION "Graphical tool for data manipulation"
        HOMEPAGE_URL "https://github.com/przemek83/volbx")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CODE_COVERAGE "Collect coverage" OFF)

if(CODE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 --coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 --coverage")
endif()

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

add_subdirectory(src/GUI)
add_subdirectory(src/Import)
add_subdirectory(src/Shared)
add_subdirectory(src/Common)
add_subdirectory(src/Datasets)
add_subdirectory(src/ModelsAndViews)
add_subdirectory(src/Export)

set(SOURCES
    src/Volbx/main.cpp
    src/Volbx/Volbx.ico
    src/Volbx/VolbxProperties.rc
)
qt_add_resources(SOURCES Resources/Resources.qrc)

# remove background console window.
if(WIN32)
    set(GUI_TYPE WIN32)
endif(WIN32)
if(APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
endif(APPLE)

add_executable(${PROJECT_NAME} ${GUI_TYPE} ${SOURCES})

target_link_libraries(${PROJECT_NAME} common gui Qt6::Widgets Qt6::Network)

add_subdirectory(src/Update)

enable_testing()
add_subdirectory(Tests)
