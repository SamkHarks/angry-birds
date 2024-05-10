#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#ifdef _WIN32
    #include <windows.h>
#elif __APPLE__
    #include <mach-o/dyld.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

std::string get_executable_path();

#endif // UTILS_HPP