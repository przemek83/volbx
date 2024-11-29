project(export)

set(SOURCES
    ExportImage.cpp
    ExportVbx.cpp
)

set(HEADERS
    ExportImage.h
    ExportVbx.h
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} shared qwtble Qt6::Core QuaZip::QuaZip)
