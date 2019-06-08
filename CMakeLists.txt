cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(volbx)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)
find_package(Qt5Network CONFIG REQUIRED)
find_package(Qt5Xml CONFIG REQUIRED)
find_package(Qt5Test CONFIG REQUIRED)

add_definitions( ${QT_DEFINITIONS} )

# Zlib
configure_file(CMakeLists.txt.zlib zlib/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/zlib )

execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/zlib )

# QuaZip
configure_file(CMakeLists.txt.quazip quazip/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/quazip )

execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/quazip )

# QWT
configure_file(CMakeLists.txt.qwt qwt/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qwt )

execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qwt )

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/qwt/install)
file(COPY ${CMAKE_BINARY_DIR}/qwt/qwt/src/qwt-build/lib DESTINATION ${CMAKE_BINARY_DIR}/qwt/install)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/qwt/install/include)
file(GLOB QWT_HEADERS "${CMAKE_BINARY_DIR}/qwt/qwt/src/qwt/src/*.h" )
file(COPY ${QWT_HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/qwt/install/include)


# workaround for not working passing "CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<dir>" in ExternalProject_Add
set(workaround ON CACHE BOOL "" FORCE)


include_directories(${CMAKE_BINARY_DIR}/qwt/install/include)
include_directories(${CMAKE_BINARY_DIR}/zlib/install/include)
include_directories(${CMAKE_BINARY_DIR}/quazip/install/include)

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/qwt/install/lib)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/zlib/install/bin)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/quazip/install/lib)


include_directories(${CMAKE_SOURCE_DIR})
include_directories(Charts)
include_directories(Filters)
include_directories(GUI)
include_directories(Import)


add_subdirectory(Shared)
include_directories(Shared)

add_subdirectory(Common)
include_directories(Common)

add_subdirectory(Datasets)
include_directories(Datasets)

add_subdirectory(ModelsAndViews)
include_directories(ModelsAndViews)

add_subdirectory(DataProvider)
include_directories(DataProvider)

file(GLOB SOURCES "Volbx/*.cpp"
    "Charts/*.cpp"
    "Filters/*.cpp"
    "GUI/*.cpp"
    "Import/*.cpp"
    "Filters/*.ui"
    "GUI/*.ui"
    "Import/*.ui"
   )


# Remove background console window.
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

target_link_libraries(${PROJECT_NAME} shared common datasets modelsAndViews dataProvider quazip5 Qt5::Core Qt5::Network Qt5::Widgets Qt5::Gui Qt5::Xml)
target_link_libraries(${PROJECT_NAME} optimized qwt)
target_link_libraries(${PROJECT_NAME} debug qwtd)

add_subdirectory(Update)

enable_testing()
add_subdirectory(Tests)
