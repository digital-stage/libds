include(FetchContent)

set(patch_command ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/signal.hpp.patch <SOURCE_DIR>/include/sigslot && git checkout <SOURCE_DIR>/include/sigslot/signal.hpp && git apply <SOURCE_DIR>/include/sigslot/signal.hpp.patch)
FetchContent_Declare(
        sigslot
        GIT_REPOSITORY https://github.com/palacaze/sigslot
        GIT_TAG e41c6e62edfa67b6402b17a627dca698a14f47a8 # v1.2.1
        PATCH_COMMAND ${patch_command}
)
FetchContent_MakeAvailable(sigslot)