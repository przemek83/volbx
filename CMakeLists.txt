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
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/zlib/install/lib)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/quazip/install/lib)

include_directories(Charts)
include_directories(Common)
include_directories(DataProvider)
include_directories(Datasets)
include_directories(Filters)
include_directories(GUI)
include_directories(Import)
include_directories(ModelsAndViews)
include_directories(Shared)

file(GLOB SOURCES "Volbx/*.cpp"
    "Charts/*.cpp"
    "Common/*.cpp"
    "DataProvider/*.cpp"
    "Datasets/*.cpp"
    "Filters/*.cpp"
    "GUI/*.cpp"
    "Import/*.cpp"
    "ModelsAndViews/*.cpp"
    "Shared/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries( ${PROJECT_NAME} zlib quazip5 qwt Qt5::Core Qt5::Network Qt5::Widgets Qt5::Gui Qt5::Xml)
