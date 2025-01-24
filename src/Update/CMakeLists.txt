#For app name containing "update", "install" or "setup" Windows need admin rights...
#Workaround it using different name.
project(VersionCheck)

set(SOURCES
    main.cpp
    Update.h
    Update.cpp
    UpdateProperties.rc
)

set(UI
    Update.ui
)
source_group("UI Files" FILES ${UI})

# Remove background console window.
if(WIN32)
    set(GUI_TYPE WIN32)
endif(WIN32)
if(APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
endif(APPLE)

add_executable(${PROJECT_NAME} ${GUI_TYPE} ${SOURCES} ${UI})

target_link_libraries(${PROJECT_NAME} shared Qt6::Widgets Qt6::Network)
