project(common)

set(${PROJECT_NAME}_SOURCES
    Configuration.cpp
    Configuration.h
    Constants.cpp
    Constants.h
    ExportData.cpp
    ExportData.h
    ExportImage.cpp
    ExportImage.h
    Formats.h
    PlotData.cpp
    PlotData.h
    Quantiles.cpp
    Quantiles.h
    SpecialColumns.h
    )

ADD_LIBRARY(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} shared wble Qt5::Core Qt5::Gui Qt5::Widgets)
