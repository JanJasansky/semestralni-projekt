cmake_minimum_required(VERSION 3.16)
project(BACCalculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt code generators
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# -----------------------------
# Qt6 discovery (cross-platform)
# -----------------------------
# Prefer passing Qt path via command line:
#   cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.10.2/mingw_64"
#
# But if you want a Windows fallback inside CMake:
if(WIN32)
    if(NOT DEFINED CMAKE_PREFIX_PATH)
        # Adjust to your Qt install root (NOT the /lib/cmake/Qt6 subfolder)
        set(CMAKE_PREFIX_PATH "C:/Qt/6.10.2/mingw_64")
    endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# -----------------------------
# Copy python folder into build
# -----------------------------
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/python"
            "${CMAKE_BINARY_DIR}/python"
    COMMENT "Copying python directory into build output"
)

# -----------------------------
# App target
# -----------------------------
add_executable(BACCalculator
    src/main.cpp
    src/MainWindow.h src/MainWindow.cpp
    src/BacModel.h src/BacModel.cpp
    src/PythonPlotter.h src/PythonPlotter.cpp
    resources.qrc
)

add_dependencies(BACCalculator copy_assets)

target_link_libraries(BACCalculator PRIVATE Qt6::Widgets)

# Optional: make sure headers in src/ are visible if you include like "MainWindow.h"
target_include_directories(BACCalculator PRIVATE "${CMAKE_SOURCE_DIR}/src")

# -----------------------------
# (Windows) Copy Qt runtime DLLs next to the exe when using MinGW
# -----------------------------
if(WIN32)
    # This is the clean modern way (CMake >= 3.21). If your CMake is older, tell me and I’ll give a fallback.
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
        add_custom_command(TARGET BACCalculator POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:BACCalculator>
                $<TARGET_FILE_DIR:BACCalculator>
            COMMAND_EXPAND_LISTS
            COMMENT "Copying runtime DLLs"
        )
    endif()
endif()
