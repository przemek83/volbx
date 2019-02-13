project(common)

file(GLOB SOURCES "*.cpp")

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} shared Qt5::Core Qt5::Gui Qt5::Widgets)
