project(modelsAndViews)

file(GLOB SOURCES "*.cpp")

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} dataProvider Qt5::Core Qt5::Widgets)
