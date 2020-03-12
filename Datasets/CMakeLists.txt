project(datasets)

set(${PROJECT_NAME}_SOURCES
    Dataset.cpp
    Dataset.h
    DatasetDefinition.cpp
    DatasetDefinition.h
    DatasetDefinitionInner.cpp
    DatasetDefinitionInner.h
    DatasetDefinitionOds.cpp
    DatasetDefinitionOds.h
    DatasetDefinitionSpreadsheet.cpp
    DatasetDefinitionSpreadsheet.h
    DatasetDefinitionXlsx.cpp
    DatasetDefinitionXlsx.h
    DatasetInner.cpp
    DatasetInner.h
    DatasetSpreadsheet.cpp
    DatasetSpreadsheet.h
    )

ADD_LIBRARY(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} wble Qt5::Core Qt5::Widgets Qt5::Xml)
