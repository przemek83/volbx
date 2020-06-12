project(tests)

set(${PROJECT_NAME}_SOURCES
    Common.cpp
    Common.h
    ConfigurationTest.cpp
    ConfigurationTest.h
    InnerTests.cpp
    InnerTests.h
    SpreadsheetsTest.cpp
    SpreadsheetsTest.h
    Tests.cpp
    PlotDataProviderTest.h
    PlotDataProviderTest.cpp
    )

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} common datasets modelsAndViews Qt5::Xml Qt5::Test Qt5::Core)
target_link_libraries(${PROJECT_NAME} optimized Qt5Quazip)
target_link_libraries(${PROJECT_NAME} debug Qt5Quazipd)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/TestFiles $<TARGET_FILE_DIR:tests>)

add_test(NAME tests COMMAND tests)
