include(FetchContent)

FetchContent_Declare(
    wble
    GIT_REPOSITORY https://github.com/przemek83/wble.git
    GIT_TAG 3f751104c684a7175ef5686454fa54158d5132d8
)

FetchContent_MakeAvailable(wble)
