project(tests)

set(SOURCES
    Common.cpp
    ConfigurationTest.cpp
    InnerTests.cpp
    SpreadsheetsTest.cpp
    Tests.cpp
    PlotDataProviderTest.cpp
    FilteringProxyModelTest.cpp
    DetailedSpreadsheetsTest.cpp
    DatasetDummy.cpp
    DatasetTest.cpp
    DatasetCommon.cpp
)
qt_add_resources(SOURCES testResources.qrc)

set(HEADERS
    Common.h
    ConfigurationTest.h
    InnerTests.h
    SpreadsheetsTest.h
    PlotDataProviderTest.h
    FilteringProxyModelTest.h
    DetailedSpreadsheetsTest.h
    DatasetDummy.h
    DatasetTest.h
    DatasetCommon.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} common datasets modelsAndViews export Qt6::Xml Qt6::Test Qt6::Core)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/TestFiles/Data" "$<TARGET_FILE_DIR:tests>/Data")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy 
                   "${CMAKE_CURRENT_SOURCE_DIR}/TestFiles/config" "$<TARGET_FILE_DIR:tests>")
           
add_test(NAME tests COMMAND tests)
