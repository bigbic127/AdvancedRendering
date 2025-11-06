#pragma once
#include <string>
#include <iostream>
#include <filesystem>
#ifdef _WIN32
    #include <windows.h>
    static std::string GetExecutableDir()
    {
        char path[1024];
        GetModuleFileNameA(NULL, path, sizeof(path));
        std::filesystem::path exePath(path);
        return exePath.parent_path().string();
    }
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    static std::string GetExecutableDir()
    {
        char path[1024];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) != 0) {
            std::cerr << "Path buffer too small!" << std::endl;
            return "";
        }
        return std::filesystem::path(path).parent_path().string();
    }
#endif

static std::string GetFullPath(const std::string& path)
{
    std::string p = path;
    std::string root = GetExecutableDir();
    p = root + path;
    return p;
}