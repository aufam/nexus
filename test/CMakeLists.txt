find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

if (NOT GTEST_FOUND)
message(FATAL_ERROR "Cannot find gtest")
endif()
enable_testing()

file(GLOB_RECURSE TEST_SRCS *.*)
add_executable(test_all ${TEST_SRCS})

target_include_directories(test_all PRIVATE
	${CMAKE_HOME_DIRECTORY}/include
	${GTEST_SOURCE_DIR}/include 
	${GTEST_SOURCE_DIR}
)

target_compile_options(test_all PRIVATE 
	-Wno-literal-suffix 
	-Wall 
	-Wextra 
)

target_link_libraries(test_all PRIVATE
	m
	Threads::Threads
	${PROJECT_NAME}
	${GTEST_LIBRARIES} 
	${GTEST_MAIN_LIBRARIES}
)
