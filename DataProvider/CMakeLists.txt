project(dataProvider)

file(GLOB SOURCES "*.cpp")

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} common Qt5::Core Qt5::Widgets)# Qt5::Core Qt5::Widgets)
