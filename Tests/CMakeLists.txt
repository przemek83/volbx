project(tests)

qt5_add_resources(RESOURCES_TESTS testResources.qrc)
add_custom_target(generatedTest_qrc DEPENDS ${RESOURCES_TESTS})

set(${PROJECT_NAME}_SOURCES
    qrc_testResources.cpp
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
    FilteringProxyModelTest.h
    FilteringProxyModelTest.cpp
    DetailedSpreadsheetsTest.cpp
    DetailedSpreadsheetsTest.h
    )

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

add_dependencies(${PROJECT_NAME} generatedTest_qrc)

target_link_libraries(${PROJECT_NAME} common datasets modelsAndViews export Qt5::Xml Qt5::Test Qt5::Core)
target_link_libraries(${PROJECT_NAME} optimized Qt5Quazip)
target_link_libraries(${PROJECT_NAME} debug Qt5Quazipd)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/TestFiles/Data $<TARGET_FILE_DIR:tests>/Data)
                   
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/TestFiles/TestSpreadsheets/detailedTests $<TARGET_FILE_DIR:tests>/detailedTests)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy 
                   ${CMAKE_CURRENT_SOURCE_DIR}/TestFiles/config $<TARGET_FILE_DIR:tests>)
           
add_test(NAME tests COMMAND tests)
