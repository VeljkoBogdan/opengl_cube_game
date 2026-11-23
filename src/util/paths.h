#ifndef PATHS_H
#define PATHS_H

#ifndef RESOURCE_DIR
#define RESOURCE_DIR "resources"
#endif
#include <string>
#include <cmrc/cmrc.hpp>

CMRC_DECLARE(res);
inline std::string loadResource(const std::string& path) {
    auto f = cmrc::res::get_filesystem().open(path);
    return std::string(f.begin(), f.end());
}

inline std::string resourcePath(const std::string& relative) {
    return std::string(RESOURCE_DIR) + "/" + relative;
}

#endif //PATHS_H
