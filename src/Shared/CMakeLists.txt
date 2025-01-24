project(shared)

set(SOURCES
    Application.h
    Application.cpp
    LoggerCheckBox.h
    LoggerCheckBox.cpp
    CommonProperties.h
    Logger.h
    Logger.cpp
    LogType.h
    Networking.h
    Networking.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Network Qt6::Widgets)
