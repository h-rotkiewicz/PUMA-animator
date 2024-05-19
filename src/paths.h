#pragma once

#include<filesystem>

#ifndef SHADERS_DIR
#error SHADERS_DIR is not defined
#endif
#ifndef RESOURCES_DIR
#error RESOURCES_DIR is not defined
#endif

namespace fs = std::filesystem;

struct Paths {
    static const fs::path shaders_vs;
    static const fs::path shaders_fs;
    static const fs::path resources_metalic;  
    static const fs::path resources_awesomeface;
};

inline const fs::path Paths::shaders_vs = fs::absolute(std::string(SHADERS_DIR) + "/transform.vs");
inline const fs::path Paths::shaders_fs = fs::absolute(std::string(SHADERS_DIR) + "/transform.fs");
inline const fs::path Paths::resources_awesomeface = fs::absolute(std::string(RESOURCES_DIR)  + "/awesomeface.png");
inline const fs::path Paths::resources_metalic = fs::absolute(std::string(RESOURCES_DIR) + "/metal-texture-25.jpg");
