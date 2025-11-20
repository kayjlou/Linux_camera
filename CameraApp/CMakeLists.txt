cmake_minimum_required(VERSION 3.10)
project(CameraApp)

# Add build options
option(SIMULATE_ALL_HARDWARE "Enable hardware simulation for development" OFF)
option(DEBUG "Enable debug mode" OFF)

if(SIMULATE_ALL_HARDWARE)
    add_compile_definitions(SIMULATE_ALL_HARDWARE)
    message(STATUS "Hardware simulation enabled")
endif()

if(DEBUG)
    add_compile_definitions(DEBUG)
    message(STATUS "Debug mode enabled")
endif()

# Enable C++17 (or newer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)

# Find OpenGL or if defined USE_GLES then find OpenGL ES2
if(USE_GLES)
    message(STATUS "Using OpenGL ES")
    pkg_check_modules(GLESv2 REQUIRED glesv2)
    add_compile_definitions(USE_GLES)
    add_compile_definitions(IMGUI_IMPL_OPENGL_ES2)
else()
    message(STATUS "Using desktop OpenGL")
    find_package(OpenGL REQUIRED)
    # Silence OpenGL deprecation warnings on macOS
    if(APPLE)
        add_compile_definitions(GL_SILENCE_DEPRECATION)
    endif()
endif()

# Find SDL2
if(APPLE)
    find_library(SDL2_LIBRARY SDL2 PATHS /Library/Frameworks /opt/homebrew/Library/Frameworks)
    set(SDL2_LIBRARIES ${SDL2_LIBRARY})
    set(SDL2_INCLUDE_DIRS /opt/homebrew/include/SDL2)
else()
    # Try pkg-config first, fallback to find_package
    pkg_check_modules(SDL2 sdl2)
    if(NOT SDL2_FOUND)
        message(STATUS "SDL2 not found via pkg-config, trying find_package...")
        find_package(SDL2 REQUIRED)
        # Handle different SDL2 package configurations
        if(TARGET SDL2::SDL2)
            set(SDL2_LIBRARIES SDL2::SDL2)
        else()
            set(SDL2_LIBRARIES ${SDL2_LIBRARIES})
        endif()
    endif()
    message(STATUS "SDL2 libraries: ${SDL2_LIBRARIES}")
    message(STATUS "SDL2 include dirs: ${SDL2_INCLUDE_DIRS}")
endif()

# Find GStreamer
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-app-1.0)
message(STATUS "GStreamer libraries: ${GSTREAMER_LIBRARIES}")
message(STATUS "GStreamer include dirs: ${GSTREAMER_INCLUDE_DIRS}")

# Find OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
    message(STATUS "OpenCV libs: ${OpenCV_LIBS}")
else()
    message(FATAL_ERROR "OpenCV not found!")
endif()

# Include directories
set(PROJECT_INCLUDE_DIRS
    ${GSTREAMER_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ErrorHandling
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Hardware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Hardware/Camera
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Hardware/CommunicationInterfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Hardware/Light
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Hardware/Temperature
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Interaction
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Overlays
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MessageHandling
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Strings
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Testing
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Versions
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image
)

# Source files
set(SRC_FILES
    src/main.cpp
    src/Fonts.cpp
    src/OverlayRenderer.cpp
    src/Video.cpp
    src/Window.cpp
    src/ErrorHandling/ErrorHandler.cpp
    src/MessageHandling/Message.cpp
    src/MessageHandling/MessageHandler.cpp
    src/Hardware/CommunicationInterfaces/I2cCommunication.cpp
    src/Hardware/CommunicationInterfaces/UartCommunication.cpp
    src/Hardware/GmslConnection.cpp
    src/Hardware/Endoscope.cpp
    src/Hardware/ControlBoard.cpp
    src/Hardware/Camera/Camera.cpp
    src/Hardware/Camera/CameraRegisterAccess.cpp
    src/Hardware/Camera/WhiteBalance.cpp
    src/Hardware/Light/LedControl.cpp
    src/Hardware/Temperature/ImxTemperature.cpp
    src/Hardware/Temperature/TemperatureMonitoring.cpp
    src/Interaction/Action.cpp
    src/Interaction/ActionHandler.cpp
    src/Interaction/ButtonHandler.cpp
    src/Overlays/ActiveCameraOverlay.cpp
    src/Overlays/BrightnessOverlay.cpp
    src/Overlays/DebugOverlay.cpp
    src/Overlays/MessageOverlay.cpp
    src/Overlays/TextOverlay.cpp
    src/Overlays/ImageOverlay.cpp
    src/Overlays/IconOverlay.cpp
    src/Overlays/RecordingModeOverlay.cpp
    src/Overlays/TimedOverlay.cpp
    src/Overlays/TimerCircle.cpp
    src/Overlays/WhiteBalanceOverlay.cpp
    src/Testing/IntegrationTesting.cpp
    src/Testing/ControlBoardLedControl.cpp
    src/Testing/HandpieceLedControl.cpp
    src/Utils/FileUtils.cpp
    src/Utils/Utils.cpp
    src/Utils/WindowUtils.cpp
    src/Versions/Versions.cpp
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/backends/imgui_impl_sdl2.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp
)

link_directories(${GSTREAMER_LIBRARY_DIRS})
add_executable(CameraApp ${SRC_FILES})

if(APPLE)
    message(STATUS "Configuring for macOS")
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(GLIB_LIBRARY NAMES gobject-2.0 PATHS /opt/homebrew/lib)
    find_library(GLIB_INCLUDE_DIR NAMES glib-2.0 PATHS /opt/homebrew/include)
    include_directories(/System/Library/Frameworks/OpenGL.framework/Headers)
    target_include_directories(CameraApp PRIVATE ${OPENGL_INCLUDE_DIR})
    target_include_directories(CameraApp PRIVATE /opt/homebrew/include)
    target_link_libraries(CameraApp ${OpenGL_LIBRARY} ${GSTREAMER_LIBRARIES} ${SDL2_LIBRARIES} ${GLIB_LIBRARY})
elseif(CMAKE_CROSSCOMPILING) #YOCTO
    message(STATUS "Configuring for Yocto Linux")
    target_include_directories(CameraApp PRIVATE ${GLESv2_INCLUDE_DIRS})
    target_link_libraries(CameraApp ${GLESv2_LIBRARIES} ${GSTREAMER_LIBRARIES} ${SDL2_LIBRARIES} ${OpenCV_LIBS})
else()
    message(STATUS "Configuring for Linux")
    target_link_libraries(CameraApp ${OPENGL_LIBRARIES} ${GSTREAMER_LIBRARIES} ${SDL2_LIBRARIES} ${OpenCV_LIBS})
endif()

target_include_directories(CameraApp PRIVATE ${PROJECT_INCLUDE_DIRS})

# Enable high warning levels but suppress warnings for third-party code
target_compile_options(CameraApp PRIVATE -Wall -Wextra -pedantic -Wno-unused-function)
