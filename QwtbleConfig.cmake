include(FetchContent)

FetchContent_Declare(
    qwtble
    GIT_REPOSITORY https://github.com/przemek83/qwtble.git
    GIT_TAG 67afcc62ee047ec1644708c296dc454e2458bb7d
)

FetchContent_MakeAvailable(qwtble)

include_directories(${FETCHCONTENT_BASE_DIR}/qwtble-build/qwt/install/include)

link_directories(${FETCHCONTENT_BASE_DIR}/qwtble-build/qwt/install/lib)