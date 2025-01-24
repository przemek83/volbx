project(common)

set(SOURCES
    Configuration.h
    Configuration.cpp
    Constants.h
    Constants.cpp
    ColumnTag.h
    DatasetUtilities.h
    DatasetUtilities.cpp
    TimeLogger.h
    TimeLogger.cpp
    FileUtilities.h
    FileUtilities.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets)
