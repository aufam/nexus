add_executable(aj-sr04 aj-sr04.cpp)
target_compile_options(aj-sr04 PRIVATE -Wall -Wextra -Wno-literal-suffix)
target_link_libraries(aj-sr04 PRIVATE ${PROJECT_NAME})

add_executable(version version.cpp)
target_compile_options(version PRIVATE -Wall -Wextra -Wno-literal-suffix)
target_link_libraries(version PRIVATE ${PROJECT_NAME})