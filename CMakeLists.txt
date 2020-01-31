cmake_minimum_required(VERSION 3.8)

project(volbx LANGUAGES CXX VERSION 2.0.0 DESCRIPTION "Graphical tool for data manipulation")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# find Qt packages
find_package(Qt5 COMPONENTS Widgets Core Gui Network Xml Test REQUIRED)
add_definitions( ${QT_DEFINITIONS} )

# Zlib
configure_file(CMakeLists.txt.zlib zlib/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/zlib)
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/zlib)

# QuaZip
configure_file(CMakeLists.txt.quazip quazip/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/quazip)
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/quazip)

# QWT
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

# wble
configure_file(CMakeLists.txt.wble wble/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/wble)
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/wble)

# copy dlls for easy launch
file(GLOB_RECURSE DLLS "${CMAKE_BINARY_DIR}/*.dll" )
file(COPY ${DLLS} DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${DLLS} DESTINATION ${CMAKE_BINARY_DIR}/Tests)

# workaround for not working passing "CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<dir>" in ExternalProject_Add
set(workaround ON CACHE BOOL "" FORCE)

include_directories(${CMAKE_BINARY_DIR}/qwt/install/include)
include_directories(${CMAKE_BINARY_DIR}/zlib/install/include)
include_directories(${CMAKE_BINARY_DIR}/quazip/install/include)
include_directories(${CMAKE_BINARY_DIR}/wble/install/include)

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/qwt/install/lib)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/zlib/install/bin)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/quazip/install/lib)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/wble/install/lib)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(Charts)
include_directories(Filters)
include_directories(GUI)
include_directories(Import)
include_directories(Shared)
include_directories(Common)
include_directories(Datasets)
include_directories(ModelsAndViews)
include_directories(DataProvider)

add_subdirectory(Shared)
add_subdirectory(Common)
add_subdirectory(Datasets)
add_subdirectory(ModelsAndViews)
add_subdirectory(DataProvider)

file(GLOB SOURCES "Volbx/*.cpp"
    "Charts/*.cpp"
    "Filters/*.cpp"
    "GUI/*.cpp"
    "Import/*.cpp"
    "Filters/*.ui"
    "GUI/*.ui"
    "Import/*.ui"
   )

# remove background console window.
IF(WIN32)
SET(GUI_TYPE WIN32)
ENDIF(WIN32)
IF (APPLE)
SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

qt5_add_resources(RESOURCES Resources/Resources.qrc)
add_custom_target(generated_qrc DEPENDS ${RESOURCES})

add_executable(${PROJECT_NAME} ${GUI_TYPE} ${SOURCES} ${RESOURCES} Volbx/VolbxProperties.rc)
add_dependencies(${PROJECT_NAME} generated_qrc)

target_link_libraries(${PROJECT_NAME} shared common datasets modelsAndViews dataProvider wble Qt5::Core Qt5::Network Qt5::Widgets Qt5::Gui Qt5::Xml)
target_link_libraries(${PROJECT_NAME} optimized qwt)
target_link_libraries(${PROJECT_NAME} debug qwtd)
target_link_libraries(${PROJECT_NAME} optimized quazip5)
target_link_libraries(${PROJECT_NAME} debug quazip5d)

add_subdirectory(Update)

enable_testing()
add_subdirectory(Tests)
