project(tests)

file(GLOB SOURCES "*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} common datasets modelsAndViews Qt5::Xml Qt5::Test Qt5::Core)
target_link_libraries(${PROJECT_NAME} optimized quazip5)
target_link_libraries(${PROJECT_NAME} debug quazip5d)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/TestFiles $<TARGET_FILE_DIR:tests>)

add_test(NAME tests COMMAND tests)
