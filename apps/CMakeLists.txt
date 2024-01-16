add_executable(version version.cpp)
target_compile_options(version PRIVATE -Wall -Wextra -Wno-literal-suffix)
target_link_libraries(version PRIVATE ${PROJECT_NAME})

add_executable(modbus_encode modbus_encode.cpp)
target_compile_options(modbus_encode PRIVATE -Wall -Wextra -Wno-literal-suffix)
target_link_libraries(modbus_encode PRIVATE ${PROJECT_NAME})

add_executable(aj-sr04 aj-sr04.cpp)
target_compile_options(aj-sr04 PRIVATE -Wall -Wextra -Wno-literal-suffix)
target_link_libraries(aj-sr04 PRIVATE ${PROJECT_NAME})

add_executable(fs50l fs50l.cpp)
target_compile_options(fs50l PRIVATE -Wall -Wextra -Wno-literal-suffix)
target_link_libraries(fs50l PRIVATE ${PROJECT_NAME})

add_executable(pzem-004t pzem-004t.cpp)
target_compile_options(pzem-004t PRIVATE -Wall -Wextra -Wno-literal-suffix)
target_link_libraries(pzem-004t PRIVATE ${PROJECT_NAME})