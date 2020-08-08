project(common)

set(${PROJECT_NAME}_SOURCES
    Configuration.cpp
    Configuration.h
    Constants.cpp
    Constants.h
    ColumnTag.h
    DatasetUtilities.cpp
    DatasetUtilities.h
    TimeLogger.cpp
    TimeLogger.h
    FileUtilities.cpp
    FileUtilities.h
    )

ADD_LIBRARY(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} shared Qt5::Core)
