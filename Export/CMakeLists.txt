project(export)

set(${PROJECT_NAME}_SOURCES
    ExportImage.cpp
    ExportImage.h
    ExportVbx.cpp
    ExportVbx.h
    )

ADD_LIBRARY(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} shared Qt5::Core wble)
