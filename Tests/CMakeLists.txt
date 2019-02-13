project(tests)

file(GLOB SOURCES "*.cpp")
add_executable(tests ${SOURCES})

target_link_libraries(tests common datasets modelsAndViews quazip5 Qt5::Xml Qt5::Test Qt5::Core)

add_custom_command(TARGET tests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/TestFiles $<TARGET_FILE_DIR:tests>)

add_test(NAME tests COMMAND tests)
