cmake_minimum_required(VERSION 3.16)

project(rpicam-gui)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src/controllers src/models src/utils src/gui)

# Enable automatic MOC generation
set(CMAKE_AUTOMOC ON)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Add source files
set(SOURCES
    src/gui/MainWindow.cpp
    src/gui/MainWindow.h # Header explizit hinzufügen
    src/models/RpiCamParams.cpp
    src/controllers/RpiCamController.cpp
    src/utils/CommandExecutor.cpp
    src/main.cpp
)

set(HEADERS
    src/gui/MainWindow.h
    src/models/RpiCamParams.h
    src/controllers/RpiCamController.h
    src/utils/CommandExecutor.h
)

# Add executable
add_executable(rpicam-gui ${SOURCES} ${HEADERS})

# Link Qt5 libraries
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)