include(FetchContent)

FetchContent_Declare(
    qwtble
    GIT_REPOSITORY https://github.com/przemek83/qwtble.git
    GIT_TAG v1.2.0
)

FetchContent_MakeAvailable(qwtble)

include_directories(${FETCHCONTENT_BASE_DIR}/qwtble-build/qwt/install/include)

link_directories(${FETCHCONTENT_BASE_DIR}/qwtble-build/qwt/install/lib)