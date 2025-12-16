add_executable(day10-1 src/main1.cpp)
target_link_libraries(day10-1 PRIVATE util::util fmt::fmt-header-only ctre::ctre)

add_executable(day10-2 src/main2.cpp)
target_link_libraries(day10-2 PRIVATE util::util fmt::fmt-header-only ctre::ctre)

add_executable(day10-2-clean src/main2_clean.cpp)
target_link_libraries(day10-2-clean PRIVATE util::util fmt::fmt-header-only ctre::ctre)
