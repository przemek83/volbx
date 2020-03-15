project(dataProvider)

set(${PROJECT_NAME}_SOURCES
    PlotDataProvider.cpp
    PlotDataProvider.h
    TransactionData.cpp
    TransactionData.h
    )

ADD_LIBRARY(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} common qwtble Qt5::Core Qt5::Widgets)
