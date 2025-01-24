project(gui)

set(SOURCES
    About.cpp
    About.h
    CheckUpdates.cpp
    CheckUpdates.h
    DataViewDock.cpp
    DataViewDock.h
    Dock.cpp
    Dock.h
    DockTitleBar.cpp
    DockTitleBar.h
    Export.cpp
    Export.h
    FiltersDock.cpp
    FiltersDock.h
    PlotDock.cpp
    PlotDock.h
    SaveDatasetAs.cpp
    SaveDatasetAs.h
    TabBar.cpp
    TabBar.h
    Tab.cpp
    Tab.h
    TabWidget.cpp
    TabWidget.h
    VolbxMain.cpp
    VolbxMain.h
)

set(UI
    About.ui
    CheckUpdates.ui
    DockTitleBar.ui
    Export.ui
    SaveDatasetAs.ui
    VolbxMain.ui
)
source_group("UI Files" FILES ${UI})


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI})

target_link_libraries(${PROJECT_NAME} modelsAndViews export import datasets Qt6::Widgets Qt6::Network eible wble qwtble QuaZip)
