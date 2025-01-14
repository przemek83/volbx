include(FetchContent)

FetchContent_Declare(
    qwtble
    GIT_REPOSITORY https://github.com/przemek83/qwtble.git
    GIT_TAG 68cfdf7aad57d80ed3244e5102ec6be3ec5c5074
)

FetchContent_MakeAvailable(qwtble)

include_directories(${FETCHCONTENT_BASE_DIR}/qwtble-build/qwt/install/include)

link_directories(${FETCHCONTENT_BASE_DIR}/qwtble-build/qwt/install/lib)