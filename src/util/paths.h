#ifndef PATHS_H
#define PATHS_H

#ifndef RESOURCE_DIR
#define RESOURCE_DIR "resources"
#endif
#include <string>

inline std::string resourcePath(const std::string& relative) {
    return std::string(RESOURCE_DIR) + "/" + relative;
}

#endif //PATHS_H
