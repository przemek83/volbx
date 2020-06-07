project(modelsAndViews)

set(${PROJECT_NAME}_SOURCES
    DataView.cpp
    DataView.h
    DateDelegate.cpp
    DateDelegate.h
    FilteringProxyModel.cpp
    FilteringProxyModel.h
    NumericDelegate.cpp
    NumericDelegate.h
    TableModel.cpp
    TableModel.h
    PlotDataProvider.cpp
    PlotDataProvider.h
    TransactionData.cpp
    TransactionData.h
    )

ADD_LIBRARY(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} qwtble common Qt5::Core Qt5::Widgets)
