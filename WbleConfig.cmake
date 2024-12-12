include(FetchContent)

FetchContent_Declare(
    wble
    GIT_REPOSITORY https://github.com/przemek83/wble.git
    GIT_TAG d91243e7696acb4f6dde28f71d264b6ac55377d9
)

FetchContent_MakeAvailable(wble)
