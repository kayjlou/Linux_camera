# Linux Camera App - AI Coding Agent Instructions

## Architecture Overview

This is an embedded Linux camera application for medical endoscopes built with C++17, OpenGL/OpenGL ES, GStreamer, and ImGui. The app runs on i.MX8 hardware with dual camera support, hardware controls, and real-time video processing.

### Core Components & Data Flow
- **Video Pipeline**: GStreamer → OpenGL textures → ImGui overlays → Display
- **Hardware Control**: UART/I2C communication with endoscopes, LEDs, temperature sensors
- **Message System**: Singleton `MessageHandler` manages UI notifications with timed visibility
- **Action System**: Thread-safe `ActionHandler` queues and processes hardware commands
- **State Management**: Global singletons with mutex protection for cross-component communication

## Key Architectural Patterns

### Overlay System
All UI elements inherit from `src/Overlays/Overlay.h` base class:
```cpp
class Overlay {
    virtual void init() {};    // Delayed initialization after OpenGL setup
    virtual void draw() {};    // ImGui rendering
    virtual bool isExpired() const { return false; };
};
```
Examples: `MessageOverlay`, `RecordingModeOverlay`, `TimedOverlay`

### Singleton Pattern for Global State
Core services use thread-safe singletons:
- `ActionHandler::getInstance()` - Hardware command queue
- `MessageHandler::getInstance()` - UI message system  
- `ErrorHandler::getInstance()` - Error state management
- `IntegrationTesting::getInstance()` - Test mode controls

### Hardware Abstraction
- `ControlBoard` manages dual `Endoscope` instances via GMSL connection
- Camera switching handled through `Video::switchCamera(int newCameraId)`
- Hardware simulation via `SIMULATE_ALL_HARDWARE` compile flag

## Build System & Cross-Platform Support

### CMake Configuration
```bash
# Desktop OpenGL build
cmake .. && make

# Embedded OpenGL ES build (Yocto)
source /path/to/yocto/environment-setup
cmake .. -DUSE_GLES=ON && make
```

### Build Flags
- `USE_GLES`: OpenGL ES instead of desktop OpenGL
- `DEBUG`: Enables debug overlays and video freeze simulation
- `SIMULATE_ALL_HARDWARE`: Mock hardware for development

### Platform-Specific Code
```cpp
#ifdef __APPLE__ 
    // macOS-specific OpenGL headers
#else
    #ifdef USE_GLES
        // Embedded Linux GLES
    #else  
        // Desktop Linux OpenGL
    #endif
#endif
```

## Development Workflows

### Testing & Debug Mode
- **Integration Testing**: `Ctrl+I` activates test mode with additional controls
- **Hardware Simulation**: Use `SIMULATE_ALL_HARDWARE` for development without hardware
- **Debug Overlays**: `#ifdef DEBUG` sections provide runtime diagnostics
- **Video Freeze Testing**: `F` key triggers error simulation in debug builds

### Key File Patterns
- **Constants**: `src/Constants.h` - Display dimensions, timing constants
- **Error Handling**: `src/ErrorHandling/` - Centralized error state with codes
- **Hardware Interfaces**: `src/Hardware/CommunicationInterfaces/` - I2C/UART abstractions
- **Message Strings**: `src/Strings/MessageStrings.h` - Localized UI text

### Critical Threading Considerations
- `ActionHandler` runs background thread for hardware commands
- `Video` class uses atomic variables for recording state
- All overlay rendering happens on main thread via ImGui
- Texture updates use mutex protection: `std::mutex textureMutex`

## Hardware-Specific Knowledge

### Camera System
- Dual endoscope support (camera 0/1) via GMSL connection
- White balance calibration per camera: `WhiteBalance whiteBalance1/whiteBalance2`
- Recording pipeline dynamically adds GStreamer elements: `queue → encoder → muxer → filesink`

### Control Mappings (from README)
- `U/N`: LED brightness control (short/long press patterns)
- `H`: Image capture / white balance
- `J`: Recording toggle / camera switch
- `1/2`: Handpiece limit switch simulation

### Yocto Integration
App deployed as custom recipe in Yocto build system:
- Binary copied to `/media/.../meta-camera-app/recipes-camera-app/`
- Rebuild with `bitbake -c clean camera-app && bitbake fsl-image-gui`
- Runtime location: `/home/root/CameraApp/CameraApp`

## Common Development Tasks

When adding new overlays, inherit from `Overlay.h` and register in `OverlayRenderer.cpp`.
When adding hardware features, extend `ControlBoard` and update action types in `Action.h`.
For UI messages, add types to `Message.h` and strings to `MessageStrings.h`.
Cross-platform code should use the established `#ifdef` patterns for OpenGL variants.

## CI/CD & Build Automation

### GitHub Actions Workflows
- **Build Camera Application**: Automated builds on push/PR with desktop and embedded targets
- **Build Yocto Image**: Full Linux image generation (manual dispatch, requires large runners)
- **Docker Build**: Containerized builds for reproducible environments
- **Release**: Automated release creation with tagged versions

### Build Environments
- **Desktop**: Ubuntu with OpenGL, hardware simulation enabled
- **Embedded**: ARM64 cross-compilation with Yocto SDK (OpenGL ES)
- **Docker**: Containerized builds for both desktop and embedded mock targets

### Deployment Pipeline
1. Development with desktop simulation (`SIMULATE_ALL_HARDWARE`)
2. Cross-compilation validation in CI
3. Full Yocto image build for integration testing
4. Release packaging for production deployment on i.MX8 hardware