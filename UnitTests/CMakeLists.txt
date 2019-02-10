project(tests)

#include_directories(${CMAKE_BINARY_DIR}/quazip/install/include)

file(GLOB SOURCES "*.cpp")
add_executable(tests ${SOURCES})

target_link_libraries(tests common datasets modelsAndViews quazip5 Qt5::Xml Qt5::Test Qt5::Core) #shared Qt5::Xml Qt5::Test)

add_test(NAME tests COMMAND tests)
