#include "glm/ext/vector_float3.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"

void ImGuiInit();

class Gui {
 public:
  glm::vec3 CurrentPointPos{};

 public:
  Gui(const Gui&)            = delete;
  Gui(Gui&&)                 = delete;
  Gui& operator=(const Gui&) = delete;
  Gui& operator=(Gui&&)      = delete;

  Gui(ImGuiIO& io, GLFWwindow* window);
  void newFrame();
  void renderImguiWindow();
  ~Gui();

 private:
  ImGuiIO& io;
};
