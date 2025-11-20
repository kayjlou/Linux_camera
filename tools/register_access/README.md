This tool can be used to write registers of the OV2740 camera 
that is connected to the Variscite DART MX8M PLUS.

This guide explains how to setup the working environment and apps to be run on the board:
https://variwiki.com/index.php?title=Yocto_Hello_World&release=mx8mp-yocto-mickledore-6.1.36_2.1.0-v1.3

Each time you open a new terminal to build the app, set up the environment 
with this command (adjust first part of path):
source ...../opt/fsl-imx-xwayland/6.1-mickledore/environment-setup-armv8a-poky-linux

To build the app, navigate to register_access folder and run this command:
$CC register_access.c -o register_access
