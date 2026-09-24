cmake_minimum_required(VERSION 3.16)

project(NoRo LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(CTest)

file(CREATE_LINK
    "${CMAKE_BINARY_DIR}/compile_commands.json"
    "${CMAKE_SOURCE_DIR}/compile_commands.json"
    SYMBOLIC
)



find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(NoRo PRIVATE ${MATH_LIBRARY})
endif()

include_directories(NoRo/)

add_executable(NoRo Example/main.c)
target_compile_definitions(NoRo PRIVATE NoRo_DEBUG_ON)
target_link_libraries(NoRo PRIVATE OpenGL::GL glfw m GLEW::GLEW ktx)

add_executable(NoRo-Tests Tests/test.c)
target_compile_definitions(NoRo-Tests PRIVATE NoRo_TEST)
target_link_libraries(NoRo-Tests PRIVATE OpenGL::GL glfw m GLEW::GLEW ktx)


add_test(NAME NoRoTest COMMAND NoRo-Tests)

set_tests_properties(NoRoTest PROPERTIES
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

include(GNUInstallDirs)
install(TARGETS NoRo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
