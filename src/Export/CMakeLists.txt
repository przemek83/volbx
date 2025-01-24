project(export)

set(SOURCES
    ExportImage.h
    ExportImage.cpp
    ExportVbx.h
    ExportVbx.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} qwtble eible QuaZip Qt6::Core Qt6::Widgets)
