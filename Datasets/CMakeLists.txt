project(datasets)

set(${PROJECT_NAME}_SOURCES
    Dataset.cpp
    Dataset.h
    DatasetOds.cpp
    DatasetOds.h
    DatasetXlsx.cpp
    DatasetXlsx.h
    DatasetInner.cpp
    DatasetInner.h
    DatasetSpreadsheet.cpp
    DatasetSpreadsheet.h
    )

ADD_LIBRARY(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} eible Threads::Threads Qt5::Core Qt5::Widgets Qt5::Xml)
