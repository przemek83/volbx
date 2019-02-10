#For app name containing "update", "install" or "setup" Windows need admin rights...
#Workaround it using different name.
project(VersionCheck)

file(GLOB SOURCES "*.cpp")

include_directories(${CMAKE_SOURCE_DIR})

# Remove background console window.
IF(WIN32)
SET(GUI_TYPE WIN32)
ENDIF(WIN32)
IF (APPLE)
SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

add_executable(${PROJECT_NAME} ${GUI_TYPE} ${SOURCES} UpdateProperties.rc)

target_link_libraries(${PROJECT_NAME} shared Qt5::Core Qt5::Network Qt5::Gui)
