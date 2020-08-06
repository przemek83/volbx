project(shared)

set(${PROJECT_NAME}_SOURCES
    Application.cpp
    Application.h
    CheckBox.cpp
    CheckBox.h
    CommonProperties.h
    Logger.cpp
    Logger.h
    LogType.h
    Networking.cpp
    Networking.h
    )

ADD_LIBRARY(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::Widgets)
