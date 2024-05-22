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
    static const fs::path resources_middle_arm_obj;
    static const fs::path resources_upper_base_obj;;
    static const fs::path resources_base_obj;
#ifdef DEBUG
    static const fs::path log_path;
#endif
};

inline const fs::path Paths::shaders_vs = fs::absolute(std::string(SHADERS_DIR) + "/transform.vs");
inline const fs::path Paths::shaders_fs = fs::absolute(std::string(SHADERS_DIR) + "/transform.fs");
inline const fs::path Paths::resources_metalic = fs::absolute(std::string(RESOURCES_DIR) + "/metal-texture-25.jpg");
inline const fs::path Paths::resources_middle_arm_obj = fs::absolute(std::string(RESOURCES_DIR) + "/middlearm.obj");
inline const fs::path Paths::resources_upper_base_obj = fs::absolute(std::string(RESOURCES_DIR) + "/UpperBase.obj");
inline const fs::path Paths::resources_base_obj = fs::absolute(std::string(RESOURCES_DIR) + "/Base.obj");
#ifdef DEBUG
inline const fs::path Paths::log_path = fs::absolute(std::string(SHADERS_DIR) + "../log.txt");
#endif
