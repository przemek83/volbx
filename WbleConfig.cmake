include(FetchContent)

FetchContent_Declare(
    wble
    GIT_REPOSITORY https://github.com/przemek83/wble.git
    GIT_TAG 10722eb2b6049ad612301f554a1fce7b77ab6045
)

FetchContent_MakeAvailable(wble)
