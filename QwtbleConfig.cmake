include(FetchContent)

FetchContent_Declare(
    qwtble
    GIT_REPOSITORY https://github.com/przemek83/qwtble.git
    GIT_TAG 053e98689130f21bba4125eb946d196403859b3d
)

FetchContent_MakeAvailable(qwtble)

include_directories(${FETCHCONTENT_BASE_DIR}/qwtble-build/qwt/install/include)

link_directories(${FETCHCONTENT_BASE_DIR}/qwtble-build/qwt/install/lib)