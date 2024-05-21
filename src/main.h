/* Why does this file exist ? well because  clang-format gets a stroke if I include clang-format off and clang-format on in the main file 
 * and refuses to format the code properly. So I put all the includes in this file and include this file  */
#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// clang-format on 

#include <iostream>
#include <vector>
#include "paths.h"
#include "ModelParams.h"
#include "shader.h"
#include "OBJ_Loader.h"
