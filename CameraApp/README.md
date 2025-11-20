# Build for OpenGL and run CameraApp on the Linux Ubuntu

- Navigate to folder CameraApp
- Create build folder: mkdir build
- Navigate to build folder: cd build
- Build and run:

  - cmake ..
  - make
  - ./CameraApp

  # Build for Opengl ES and run CameraApp of yocto Board

- Navigate to folder CameraApp
- Create build folder: mkdir build
- Navigate to build folder: cd build
- Build and run:
  - source /media/onexip/1fca541d-9a09-4a96-9e1d-25b611db41dd/opt/fsl-imx-xwayland/6.1-mickledore/environment-setup-armv8a-poky-linux
  - cmake .. -DUSE_GLES=ON
  - make
  - copy CameraApp binary File of Memorystick
  - start Board
  - ./CameraApp

# Key commands

- U short press: increase LED brightness
- U long press: enter gain adjustment mode
- N short press: decrease LED brightness
- N long press: enter gain adjustment mode
- H short press: capture image
- H long press: white balance
- J short press: start recording (press J again to stop recording) 
- J long press: switch camera
- 1: Toggle distal limit switch of handpiece 1
- 2: Toggle distal limit switch of handpiece 2
- T: open terminal
- X: read i.mx temperature
- C: reset camera
- V: show app version and Yocto Linux version
- F: trigger video freeze error state
- Q: quit the app

- Ctrl+I: Activate/deactivate integration test mode
In integration test mode additional keys:
  - D: Activate/deactivate temperate monitoring display

# build App with yocto

- source /media/onexip/1fca541d-9a09-4a96-9e1d-25b611db41dd/opt/fsl-imx-xwayland/6.1-mickledore/environment-setup-armv8a-poky-linux
- bitbake fsl-image-gui
- ./fsl-imx-xwayland-glibc-x86_64-fsl-image-gui-armv8a-imx8mp-var-dart-toolchain-6.1-mickledore.sh

- bitbake fsl-image-gui -c populate_sdk

### Orginal gst Pipeline zum Testen

- gstlaunch-1.0 v412src device=/dev/video3 ! video/x-raw,with=1920,height=1080 ! autovideosink sync=false

# build yocto image

- new Terminal
- go to dir /media/onexip/1fca541d-9a09-4a96-9e1d-25b611db41dd/var-fsl-yocto
- source setup-environment build_xwayland
- bitbake fsl-image-gui
- wait until finished

# Build Yocto Image with Forced Rebuild

```bash
# Go to your Yocto build directory
cd /media/onexip/1fca541d-9a09-4a96-9e1d-25b611db41dd/var-fsl-yocto
source setup-environment build_xwayland

# (1) Replace the binary in the layer
cp /media/onexip/1fca541d-9a09-4a96-9e1d-25b611db41dd/Modular-cu-linux-sw/CameraApp/build/CameraApp /media/onexip/1fca541d-9a09-4a96-9e1d-25b611db41dd/var-fsl-yocto/sources/meta-camera-app/recipes-camera-app/camera-app/files/CameraApp/CameraApp

# (2) Force reinstall your recipe
bitbake -c clean camera-app
bitbake -c install camera-app
bitbake -f -c package camera-app

# (3) Then rebuild the image
bitbake fsl-image-gui
````

# copy Yocto Image to sd card

- sudo fdisk -l zum ermitteln der laufwerks kennung in linux (sdd)
- goto dir /media/onexip/1fca541d-9a09-4a96-9e1d-25b611db41dd/var-fsl-yocto/build_xwayland
- zstdcat tmp/deploy/images/imx8mp-var-dart/fsl-image-gui-imx8mp-var-dart.wic.zst | sudo dd of=/dev/sdd bs=1M conv=fsync
- wait until finished

# app Folder Structure

/home/root/
├── CameraApp/
│ ├── CameraApp # <- deine Binary (z. B. durch CMake/Make erzeugt)
│ └── imgui.ini
│
├── data/ # enthält Daten, die die App verwendet
│ ├── font/
│ ├── icons/
│ └── testdata/
