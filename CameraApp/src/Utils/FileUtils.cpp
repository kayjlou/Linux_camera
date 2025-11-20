/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "FileUtils.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

FileUtils::FileUtils() {
}

FileUtils::~FileUtils() {
}

FileUtils& FileUtils::getInstance() {
    static FileUtils instance;
    return instance;
}

std::string FileUtils::getTimestampedFilename(const std::string& prefix, const std::string& extension) {
    using namespace std::chrono;
    
    if (savePath.empty()) {
        if (!checkForUsbStorageDevices()) {
            return "";
        }
    }
    
    if (!createDefaultFolder()) {
        return "";
    }

    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << savePath 
        << prefix << "_" 
        << std::put_time(std::localtime(&t), "%Y%m%d_%H%M%S")
        << "_" << std::setw(3) << std::setfill('0') << ms.count()
        << "." << extension;

    return oss.str();
}

bool FileUtils::checkForUsbStorageDevices() {
    savePath = "";

    // Run "df -h" and get the output
    FILE* fp = popen("df -h", "r");
    if (fp == nullptr) {
        printf("Failed to run df -h command\n");
        return false;
    }

    char buffer[256];

    // Read the output of "df -h"
    while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
        // Check if the line contains a mount point under /run/media/
        if (strstr(buffer, "/run/media/") != nullptr && strstr(buffer, "/rootfs") == nullptr) { 
            char device[64], mountPoint[128];
            sscanf(buffer, "%s %*s %*s %*s %*s %s", device, mountPoint);
            savePath = mountPoint;
            savePath +=  + "/" + defaultFolder + "/";
            printf("USB storage device found:%s\n", mountPoint);
            printf("savePath:%s\n", savePath.c_str());
            break; //Simply use the first one that is found
        }
    }

    pclose(fp);

    if (savePath.empty()) {
        printf("No USB storage device found.\n");
        return false;
    }

    return true;
}

bool FileUtils::createDefaultFolder() {
    try {
        if (std::filesystem::exists(savePath)) {
            return true;
        }

        if (std::filesystem::create_directory(savePath)) {
            return true;
        }
    } catch (const std::exception& e) {
        return false;
    }

    printf("Failed to create directory: %s\n", savePath.c_str());
    return false;
}

bool FileUtils::flushFile(const std::string& filePath) {
    int fd = open(filePath.c_str(), O_WRONLY);

    if (fd < 0) {
        perror("Failed to open file for flushing");
        return false;
    }

    bool success = true;
    
    if (fsync(fd) != 0) {
        perror("Failed to flush file");
        success = false;
    }

    close(fd);

    return success;
}
