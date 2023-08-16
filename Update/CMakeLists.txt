#For app name containing "update", "install" or "setup" Windows need admin rights...
#Workaround it using different name.
project(VersionCheck)

set(SOURCES
    main.cpp
    Update.cpp
    UpdateProperties.rc
)

set(HEADERS
    Update.h
    UpdateProperties.h
)

set(UI
    Update.ui
)
source_group("UI Files" FILES ${UI})

include_directories(${CMAKE_SOURCE_DIR})

# Remove background console window.
if(WIN32)
    set(GUI_TYPE WIN32)
endif(WIN32)
if(APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
endif(APPLE)

add_executable(${PROJECT_NAME} ${GUI_TYPE} ${SOURCES} ${HEADERS} ${UI})

target_link_libraries(${PROJECT_NAME} shared Qt6::Core Qt6::Network Qt6::Gui)
