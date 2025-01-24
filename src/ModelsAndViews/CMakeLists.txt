project(modelsAndViews)

set(SOURCES
    DataView.h
    DataView.cpp
    DateDelegate.h
    DateDelegate.cpp
    FilteringProxyModel.h
    FilteringProxyModel.cpp
    NumericDelegate.h
    NumericDelegate.cpp
    TableModel.h
    TableModel.cpp
    PlotDataProvider.h
    PlotDataProvider.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} shared common qwtble eible Qt6::Widgets)
