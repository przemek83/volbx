project(modelsAndViews)

set(SOURCES
    DataView.cpp
    DateDelegate.cpp
    FilteringProxyModel.cpp
    NumericDelegate.cpp
    TableModel.cpp
    PlotDataProvider.cpp
)

set(HEADERS
    DataView.h
    DateDelegate.h
    FilteringProxyModel.h
    NumericDelegate.h
    TableModel.h
    PlotDataProvider.h
    TransactionData.h
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} qwtble common shared Qt6::Core Qt6::Widgets eible)
