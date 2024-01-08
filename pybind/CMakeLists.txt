# find pybind directory
if (NOT DEFINED PYBIND_CONFIG)
    set(PYBIND_CONFIG pybind11-config)
endif()
execute_process(
    COMMAND ${PYBIND_CONFIG} --cmakedir
    OUTPUT_VARIABLE pybind11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# find python and pybind11
find_package(pybind11 REQUIRED CONFIG PATHS ${pybind11_DIR})
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# find python packages directory
if (NOT DEFINED PYTHON_PACKAGES_DIR)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/get_python_packages_dir.py
        OUTPUT_VARIABLE PYTHON_PACKAGES_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

# some messages
message("Python_INCLUDE_DIRS    : ${Python_INCLUDE_DIRS}")
message("Python_LIBRARIES       : ${Python_LIBRARIES}")
message("pybind11_INCLUDE_DIRS  : ${pybind11_INCLUDE_DIRS}")
message("PYTHON_PACKAGES_DIR    : ${PYTHON_PACKAGES_DIR}")

# build python module
file(GLOB_RECURSE PYBIND_SOURCES CONFIGURE_DEPENDS *.*)
pybind11_add_module(py_${PROJECT_NAME} ${SOURCES} ${PYBIND_SOURCES})   
get_target_property(TARGET_INCLUDE_DIRS ${PROJECT_NAME} INCLUDE_DIRECTORIES)
get_target_property(TARGET_LINK_LIBRARIES ${PROJECT_NAME} LINK_LIBRARIES)
get_target_property(TARGET_DEFINITIONS ${PROJECT_NAME} COMPILE_DEFINITIONS)

target_compile_options(py_${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wno-literal-suffix
)

target_include_directories(py_${PROJECT_NAME} 
    PUBLIC ${TARGET_INCLUDE_DIRS}
    PUBLIC ${Python_INCLUDE_DIRS}
    PUBLIC ${pybind11_INCLUDE_DIRS}
)

target_link_libraries(py_${PROJECT_NAME}
    PUBLIC ${TARGET_LINK_LIBRARIES}
    PUBLIC ${Python_LIBRARIES}
)

target_compile_definitions(py_${PROJECT_NAME}
    PUBLIC ${TARGET_DEFINITIONS}
)

set_target_properties(py_${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# install python module
install(TARGETS py_${PROJECT_NAME} DESTINATION ${PYTHON_PACKAGES_DIR})
