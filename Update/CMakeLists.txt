#For app name containing "update", "install" or "setup" Windows need admin rights...
#Workaround it using different name.
project(VersionCheck)

set(${PROJECT_NAME}_SOURCES
    main.cpp
    Update.cpp
    Update.h
    Update.ui
    UpdateProperties.h
    UpdateProperties.rc
    )

include_directories(${CMAKE_SOURCE_DIR})

# Remove background console window.
if(WIN32)
    set(GUI_TYPE WIN32)
endif(WIN32)
if(APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
endif(APPLE)

add_executable(${PROJECT_NAME} ${GUI_TYPE} ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} shared Qt5::Core Qt5::Network Qt5::Gui)
