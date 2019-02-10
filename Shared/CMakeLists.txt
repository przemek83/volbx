project(shared)

file(GLOB SOURCES "*.cpp")

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::Widgets)
