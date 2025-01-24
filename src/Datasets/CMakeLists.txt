project(datasets)

set(SOURCES
    Dataset.h
    Dataset.cpp
    DatasetOds.h
    DatasetOds.cpp
    DatasetXlsx.h
    DatasetXlsx.cpp
    DatasetInner.h
    DatasetInner.cpp
    DatasetSpreadsheet.h
    DatasetSpreadsheet.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} eible QuaZip Qt6::Xml Qt6::Widgets)
