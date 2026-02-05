cmake_minimum_required(VERSION 3.16)
project(BACCalculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Explicitly include Qt directories
include_directories(/opt/homebrew/Cellar/qt/6.10.1/include)
include_directories(/opt/homebrew/Cellar/qt/6.10.1/include/QtWidgets)
include_directories(/opt/homebrew/Cellar/qt/6.10.1/include/QtCore)

# Ensure Qt include directories are properly set
include_directories(${Qt6Widgets_INCLUDE_DIRS})
link_directories(${Qt6Widgets_LIBRARY_DIRS})

# Add definitions for Qt
add_definitions(${Qt6Widgets_DEFINITIONS})

# Ensure Qt meta-object compiler (moc) is enabled for MainWindow
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(BACCalculator
    src/main.cpp
    src/MainWindow.h src/MainWindow.cpp
    src/BacModel.h src/BacModel.cpp
    src/PythonPlotter.h src/PythonPlotter.cpp
)

target_link_libraries(BACCalculator PRIVATE Qt6::Widgets)
