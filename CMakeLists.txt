cmake_minimum_required(VERSION 3.16)

project(NoRo LANGUAGES C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(NoRo/)

add_executable(NoRo src/main.c)

target_compile_definitions(NoRo PRIVATE NoRo_DEBUG_ON)

target_link_libraries(NoRo PRIVATE GL glfw m GLEW ktx)

include(GNUInstallDirs)
install(TARGETS NoRo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
