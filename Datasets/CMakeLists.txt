project(datasets)

set(SOURCES
    Dataset.cpp
    DatasetOds.cpp
    DatasetXlsx.cpp
    DatasetInner.cpp
    DatasetSpreadsheet.cpp
)

set(HEADERS
    Dataset.h
    DatasetOds.h
    DatasetXlsx.h
    DatasetInner.h
    DatasetSpreadsheet.h
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} shared common Qt6::Core Qt6::Xml eible QuaZip)
