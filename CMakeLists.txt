cmake_minimum_required(VERSION 3.16)

project(rpicam-gui)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
set(CMAKE_BUILD_TYPE Debug)

# Include directories
include_directories(src/controllers src/models src/utils src/gui)

# Enable automatic MOC generation
set(CMAKE_AUTOMOC ON)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Add source files
set(SOURCES
    src/gui/MainWindow.cpp
    src/gui/SelectionOverlay.cpp
    src/gui/GuiSetupDialog.cpp # Aktualisierter Dateiname
    src/models/RpiCamParams.cpp
    src/controllers/RpiCamController.cpp
    src/utils/CommandExecutor.cpp
    src/utils/DebugLogger.cpp
    src/main.cpp
)

set(HEADERS
    src/gui/MainWindow.h
    src/gui/CustomLineEdit.h
    src/gui/GuiSetupDialog.h # Aktualisierter Dateiname
    src/models/RpiCamParams.h
    src/controllers/RpiCamController.h
    src/utils/CommandExecutor.h
    src/utils/DebugLogger.h
    src/Version.h
)

# Add executable
add_executable(rpicam-gui ${SOURCES} ${HEADERS})

# Link Qt5 libraries
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

# Testprogramm hinzufügen
add_executable(test-app src/gui/test.cpp)

# Qt-Bibliotheken für das Testprogramm verlinken
target_link_libraries(test-app Qt5::Widgets)

# Setze den Include-Pfad
target_include_directories(${PROJECT_NAME} PRIVATE src/gui src/utils)