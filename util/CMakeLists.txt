add_library(util INTERFACE)
add_library(util::util ALIAS util)
target_include_directories(util
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(util INTERFACE fmt::fmt-header-only)

