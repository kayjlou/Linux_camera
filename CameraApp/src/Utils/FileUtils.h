/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>

class FileUtils {
public:
    FileUtils(const FileUtils&) = delete;
    FileUtils& operator=(const FileUtils&) = delete;
    static FileUtils& getInstance();
    std::string getTimestampedFilename(const std::string& prefix, const std::string& extension);
    bool checkForUsbStorageDevices();
    bool flushFile(const std::string& filePath);
private:
    inline static const std::string defaultFolder = "ModularControlUnit";

    std::string savePath = "";

    FileUtils();
    ~FileUtils();
    bool createDefaultFolder();
};

#endif