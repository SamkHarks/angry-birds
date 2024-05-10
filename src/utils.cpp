#include "utils.hpp"

std::string get_executable_path() {
    std::string path;
    #ifdef _WIN32
        char result[MAX_PATH];
        DWORD count = GetModuleFileName(NULL, result, MAX_PATH);
        path = std::string(result, (count > 0) ? count : 0);
    #elif __APPLE__
        char result[PATH_MAX];
        uint32_t count = sizeof(result);
        if (_NSGetExecutablePath(result, &count) == 0)
            path = std::string(result);
    #else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        path = std::string(result, (count > 0) ? count : 0);
    #endif
    return path.substr(0, path.find_last_of("/\\"));
}