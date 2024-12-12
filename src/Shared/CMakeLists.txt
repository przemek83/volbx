project(shared)

set(SOURCES
    Application.cpp
    LoggerCheckBox.cpp
    Logger.cpp
    Networking.cpp
)

set(HEADERS
    Application.h
    LoggerCheckBox.h
    CommonProperties.h
    Logger.h
    LogType.h
    Networking.h
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Network Qt6::Widgets)
