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

include_directories(NoRo/)

add_executable(NoRo Example/main.c)
add_executable(NoRo-Tests Tests/test.c)

target_compile_definitions(NoRo PRIVATE NoRo_DEBUG_ON)
target_compile_definitions(NoRo-Tests PRIVATE NoRo_TEST)

target_link_libraries(NoRo PRIVATE OpenGL::GL glfw GLEW::GLEW)
target_link_libraries(NoRo-Tests PRIVATE OpenGL::GL glfw GLEW::GLEW)

if(WIN32)
    find_package(Ktx CONFIG REQUIRED)

    target_link_libraries(NoRo PRIVATE KTX::ktx)
    target_link_libraries(NoRo-Tests PRIVATE KTX::ktx)
else()
    target_link_libraries(NoRo PRIVATE ktx)
    target_link_libraries(NoRo-Tests PRIVATE ktx)
endif()

if(NOT WIN32)
    target_link_libraries(NoRo PRIVATE m)
    target_link_libraries(NoRo-Tests PRIVATE m)
endif()


add_test(NAME NoRoTest COMMAND NoRo-Tests)

set_tests_properties(NoRoTest PROPERTIES
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

include(GNUInstallDirs)
install(TARGETS NoRo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
