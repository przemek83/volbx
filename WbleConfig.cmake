include(FetchContent)

FetchContent_Declare(
    wble
    GIT_REPOSITORY https://github.com/przemek83/wble.git
    GIT_TAG v1.2.0
)

FetchContent_MakeAvailable(wble)
