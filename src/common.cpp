#include "common.h"
void framebuffer_size_callback(GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); }

void CheckForErrors(std::string_view message) {
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) std::cerr << message << " || OpenGL Error: " << error << std::endl;
}
void preRender() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow *init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(Settings::SCR_WIDTH, Settings::SCR_HEIGHT, "Puma", NULL, NULL);
  if (window == nullptr) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw std::runtime_error("Failed to init GLAD ");
  glEnable(GL_DEPTH_TEST);
  return window;
}

std::tuple<GLuint, GLuint, GLuint> bindBuffers(const std::vector<objl::Vertex> &vertices, const std::vector<unsigned int> &indices) {
  GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(objl::Vertex), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
  CheckForErrors("bindBuffers");

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void *)offsetof(objl::Vertex, Position));
  glEnableVertexAttribArray(0);
  // normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void *)offsetof(objl::Vertex, Normal));
  glEnableVertexAttribArray(1);
  // Tex coordinate attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void *)offsetof(objl::Vertex, TextureCoordinate));
  glEnableVertexAttribArray(2);
  CheckForErrors("Attributes");

  glBindVertexArray(0);  // Unbind VAO
  return {VAO, VBO, EBO};
}

std::tuple<GLuint, GLuint, GLuint, std::size_t> load_vxo(std::string_view path) {
  objl::Loader Loader;
  if (!Loader.LoadFile(path.data())) {
    std::cerr << "Failed to load OBJ file." << path.data() << std::endl;
    throw std::runtime_error("Failed to load OBJ file.");
  }
  auto indices         = Loader.LoadedIndices;
  auto [VAO, VBO, EBO] = bindBuffers(Loader.LoadedVertices, indices);
  return {VAO, VBO, EBO, indices.size()};
}
void Part::Render() const {
  glBindVertexArray(VAO);
  CheckForErrors(std::string("Binding VAO: ") + std::to_string(VAO));
  glDrawElements(GL_TRIANGLE_STRIP,
                 ebo_size,
                 GL_UNSIGNED_INT,
                 0);  // traiangle strip Works better ???? why ??
                      // GL_LINES_ADJACENCY also looks cool
                      // GL_LINES also looks cool
  CheckForErrors("Drawing elements");
}

Part::~Part() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

Part::Part(std::string_view path) {
  auto [VAO_, VBO_, EBO_, Size] = load_vxo(path);
  VAO                           = VAO_;
  VBO                           = VBO_;
  EBO                           = EBO_;
  ebo_size                      = Size;
}

Part &Part::operator=(Part &&other) {
  VAO      = std::exchange(other.VAO, 0);
  VBO      = std::exchange(other.VBO, 0);
  EBO      = std::exchange(other.EBO, 0);
  ebo_size = std::exchange(other.ebo_size, 0);
  return *this;
}

Part::Part(Part &&other) {
  VAO      = std::exchange(other.VAO, 0);
  VBO      = std::exchange(other.VBO, 0);
  EBO      = std::exchange(other.EBO, 0);
  ebo_size = std::exchange(other.ebo_size, 0);
}

