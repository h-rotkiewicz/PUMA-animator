#pragma once

#include <cstring>
#ifndef SHADERS_DIR
#error SHADERS_DIR is not defined
#endif
#ifndef RESOURCES_DIR
#error RESOURCES_DIR is not defined
#endif

#define FILEPATH(DIR, FILE) DIR "/" FILE

struct Paths {
  static constexpr const char* shaders_vs               = FILEPATH(SHADERS_DIR, "transform.vs");
  static constexpr const char* shaders_fs               = FILEPATH(SHADERS_DIR, "transform.fs");
  static constexpr const char* resources_metalic        = FILEPATH(RESOURCES_DIR, "metal-texture-25.jpg");
  static constexpr const char* resources_middle_arm_obj = FILEPATH(RESOURCES_DIR, "middlearm.obj");
  static constexpr const char* resources_upper_base_obj = FILEPATH(RESOURCES_DIR, "UpperBase.obj");
  static constexpr const char* resources_base_obj       = FILEPATH(RESOURCES_DIR, "Base.obj");
  static constexpr const char* resources_joint       = FILEPATH(RESOURCES_DIR, "joint.obj");
  static constexpr const char* resources_Forearm       = FILEPATH(RESOURCES_DIR, "Forearm.obj");
  static constexpr const char* resources_hand       = FILEPATH(RESOURCES_DIR, "hand.obj");
  static constexpr const char* shaders_fs_debug = FILEPATH(RESOURCES_DIR, "shaders_debug.fs");
  static constexpr const char* shaders_vs_debug = FILEPATH(RESOURCES_DIR, "shaders_debug.vs");

};
