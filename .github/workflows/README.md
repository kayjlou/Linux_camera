# GitHub Actions CI/CD Workflows

This repository includes streamlined GitHub Actions workflows for building and releasing the Camera App for embedded Linux systems.

## Available Workflows

### 1. Build Camera Application (`build-camera-app.yml`)

**Purpose**: Main CI/CD workflow for building and testing the camera application.

**Triggers**: 
- Push to `main` branch
- Pull requests to `main`
- Manual dispatch

**Features**:
- Builds for Ubuntu 22.04 with complete dependency resolution
- Includes OpenGL, GStreamer, SDL2, and OpenCV dependencies
- Hardware simulation mode for CI environments
- Artifact upload for successful builds

### 2. Build Yocto Image (`build-yocto-image.yml`)

**Purpose**: Generates complete embedded Linux image for i.MX8 hardware.

**Triggers**: 
- Manual dispatch only (resource intensive)

**Features**:
- Full Yocto Linux build with camera app integration
- Requires large runners (8+ cores, 32GB+ RAM)
- Produces bootable image for production deployment

### 3. Docker Build (`build-docker.yml`)

**Purpose**: Containerized builds for reproducible development environments.

**Triggers**: 
- Manual dispatch
- Push to main (optional)

**Features**:
- Desktop and embedded build environments
- Consistent dependency versions
- Portable development setup

### 4. Release (`release.yml`)

**Purpose**: Automated release creation and asset packaging.

**Triggers**: 
- Push of version tags (v*)

**Features**:
- Automated changelog generation
- Binary artifact packaging
- Release notes with build information

**Usage**:
```bash
# Automatic on push/PR
git push origin main

# Manual dispatch
# Go to Actions tab → "Build Camera Application" → "Run workflow"
# Select build type: desktop or embedded
```

### 2. Build Yocto Linux Image (`build-yocto-image.yml`)

**Purpose**: Builds a complete Yocto Linux image with the camera application integrated.

**Triggers**: Manual dispatch only (due to resource requirements)

**Configuration Options**:
- Yocto branch/release (default: mickledore)
- Target machine (imx8mp-var-dart, imx8mm-var-dart)
- Image type (fsl-image-gui, core-image-minimal, core-image-weston)
- Build SDK option

**Requirements**:
- Large runner (8+ GB RAM, 100+ GB storage)
- 4-8 hours build time
- Significant network bandwidth

**Usage**:
```bash
# Manual dispatch only
# Go to Actions tab → "Build Yocto Linux Image" → "Run workflow"
# Configure options as needed
```

### 3. Docker Build (`build-docker.yml`)

**Purpose**: Builds the application in containerized environments for reproducible builds.

**Triggers**:
- Push to `main`
- Pull requests to `main`
- Manual dispatch

**Build Matrix**:
- `desktop`: Ubuntu-based desktop build
- `embedded-mock`: Cross-compilation simulation (requires actual Yocto SDK for production)

**Artifacts**: Docker images and extracted binaries

### 4. Release (`release.yml`)

**Purpose**: Creates GitHub releases with built binaries for both desktop and embedded targets.

**Triggers**:
- Push of version tags (`v*.*.*`)
- Manual dispatch with tag specification

**Artifacts**: 
- `camera-app-desktop-v*.tar.gz`: Desktop Linux binary
- `camera-app-embedded-v*.tar.gz`: ARM64 binary (with installation instructions)

## Setting Up Workflows

### Prerequisites

1. **For Yocto Builds**: 
   - Set up a self-hosted runner with Yocto environment
   - Install Variscite BSP and configure build environment
   - Update paths in `build-yocto-image.yml` to match your setup

2. **For Cross-Compilation**:
   - Provide Yocto SDK in workflow or use cached SDK
   - Update SDK paths in workflows

### Configuration Steps

1. **Update Yocto Paths**:
   ```yaml
   # In build-yocto-image.yml, update these paths:
   DL_DIR: "/your/path/to/yocto-cache/downloads"
   SSTATE_DIR: "/your/path/to/yocto-cache/sstate-cache"
   ```

2. **Configure Self-Hosted Runner** (for Yocto builds):
   ```bash
   # On your build server
   ./config.sh --url https://github.com/your-username/your-repo --token YOUR_TOKEN
   ./run.sh
   ```

3. **Enable Large Runners** (for GitHub-hosted Yocto builds):
   - Requires GitHub Team/Enterprise plan
   - Update runner specification in workflows

## Workflow Details

### Desktop Build
- Uses standard Ubuntu runners
- Installs GStreamer, OpenGL, and other dependencies
- Enables hardware simulation mode
- ~5-10 minute build time

### Embedded Build
- Requires Yocto SDK or cross-compilation toolchain
- Builds with OpenGL ES support
- Creates ARM64 binaries for i.MX8 hardware
- ~30 minutes with proper SDK

### Yocto Image Build
- Complete Linux system build
- Integrates camera app as custom layer
- Produces `.wic.zst` image files for SD card flashing
- 4-8 hours build time, requires 100+ GB storage

## Customization

### Adding Custom Yocto Layers
```bash
# In build-yocto-image.yml, add to bblayers.conf:
echo 'BBLAYERS += "${BSPDIR}/sources/meta-your-layer"' >> conf/bblayers.conf
```

### Modifying Build Configuration
```bash
# Add to local.conf in workflow:
cat >> conf/local.conf << 'EOF'
# Your custom configurations
MACHINE = "your-machine"
IMAGE_INSTALL:append = " your-packages"
EOF
```

### Custom Docker Environments
- Modify `docker/Dockerfile.desktop` for desktop environment
- Update `docker/Dockerfile.embedded` for cross-compilation setup

## Troubleshooting

### Common Issues

1. **Yocto Build Fails - Storage**:
   - Use `maximize-build-space` action
   - Clean cache regularly
   - Use external storage for large builds

2. **Cross-Compilation Missing Dependencies**:
   - Ensure Yocto SDK is properly installed
   - Source environment setup script
   - Check toolchain paths

3. **Docker Build Fails**:
   - Verify base image compatibility
   - Check package availability
   - Update package manager cache

### Performance Optimization

1. **Use Build Caches**:
   - Yocto: Cache downloads and sstate
   - Docker: Use GitHub Actions cache
   - CMake: Cache build dependencies

2. **Parallel Builds**:
   ```yaml
   BB_NUMBER_THREADS: "$(nproc)"
   PARALLEL_MAKE: "-j $(nproc)"
   ```

3. **Self-Hosted Runners**:
   - Dedicated build servers with SSD storage
   - Local Yocto mirror servers
   - Persistent build environments

## Integration with Development Workflow

### Continuous Integration
- All builds triggered on PRs for validation
- Desktop builds for quick feedback
- Full Yocto builds on release branches

### Release Process
1. Tag version: `git tag v1.0.0 && git push --tags`
2. Workflow creates release with binaries
3. Download and flash image to SD card
4. Deploy to target hardware

### Development Cycle
1. Develop on desktop with simulation
2. Test cross-compilation in PR
3. Full Yocto build for integration testing
4. Release for production deployment