cmake_minimum_required(VERSION 3.16)

project(Weave
    VERSION 0.1.0
    LANGUAGES CXX
)

# --------------------------------------------------
# C++ standard
# --------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------
# MSVC hot reload / debug info (safe & modern)
# --------------------------------------------------
if (MSVC AND POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<CONFIG:Debug,RelWithDebInfo>,EditAndContinue,ProgramDatabase>"
    )
endif()

# --------------------------------------------------
# Sources
# --------------------------------------------------
file(GLOB_RECURSE WEAVE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# --------------------------------------------------
# Target
# --------------------------------------------------
add_executable(Weave
    ${WEAVE_SOURCES}
)

# --------------------------------------------------
# Include directories (target-based, modern CMake)
# --------------------------------------------------
target_include_directories(Weave
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# --------------------------------------------------
# Warnings (optional but recommended)
# --------------------------------------------------
if (MSVC)
    target_compile_options(Weave PRIVATE /W4)
else()
    target_compile_options(Weave PRIVATE -Wall -Wextra -Wpedantic)
endif()
