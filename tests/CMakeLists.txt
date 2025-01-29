SET(PROJECT_TESTS ${PROJECT_NAME}-tests)

set(${PROJECT_TESTS}_SOURCES
    Common.h
    Common.cpp
    ConfigurationTest.h
    ConfigurationTest.cpp
    InnerTests.h
    InnerTests.cpp
    SpreadsheetsTest.h
    SpreadsheetsTest.cpp
    Tests.cpp
    PlotDataProviderTest.h
    PlotDataProviderTest.cpp
    FilteringProxyModelTest.h
    FilteringProxyModelTest.cpp
    DetailedSpreadsheetsTest.h
    DetailedSpreadsheetsTest.cpp
    DatasetDummy.h
    DatasetDummy.cpp
    DatasetTest.h
    DatasetTest.cpp
    DatasetCommon.h
    DatasetCommon.cpp
    DatasetVisualizationTest.h
    DatasetVisualizationTest.cpp
    ConstantsTest.h
    ConstantsTest.cpp
)
qt_add_resources(${PROJECT_TESTS}_SOURCES testResources.qrc)

add_executable(${PROJECT_TESTS} ${${PROJECT_TESTS}_SOURCES})

target_link_libraries(${PROJECT_TESTS} datasets common eible export modelsAndViews import QuaZip qwtble Qt6::Core Qt6::Test Qt6::Gui Qt6::Xml Qt6::Widgets)

add_custom_command(TARGET ${PROJECT_TESTS} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/TestFiles/Data" "$<TARGET_FILE_DIR:${PROJECT_TESTS}>/Data")

add_custom_command(TARGET ${PROJECT_TESTS} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy 
                   "${CMAKE_CURRENT_SOURCE_DIR}/TestFiles/config" "$<TARGET_FILE_DIR:${PROJECT_TESTS}>")

if (WIN32)
    add_custom_command(TARGET ${PROJECT_TESTS} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/zlib/install/bin" "$<TARGET_FILE_DIR:${PROJECT_TESTS}>")
    add_custom_command(TARGET ${PROJECT_TESTS} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${FETCHCONTENT_BASE_DIR}/qwtble-build/qwt/install/lib" "$<TARGET_FILE_DIR:${PROJECT_TESTS}>")
endif()
           
add_test(NAME ${PROJECT_TESTS} COMMAND ${PROJECT_TESTS})
set_property(TEST ${PROJECT_TESTS} PROPERTY FAIL_REGULAR_EXPRESSION "FAIL!")
