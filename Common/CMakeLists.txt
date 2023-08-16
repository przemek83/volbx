project(common)

set(SOURCES
    Configuration.cpp
    Constants.cpp
    DatasetUtilities.cpp
    TimeLogger.cpp
    FileUtilities.cpp
)

set(HEADERS
    Configuration.h
    Constants.h
    ColumnTag.h
    DatasetUtilities.h
    TimeLogger.h
    FileUtilities.h
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} shared Qt6::Core)
