#include "Rendering.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


using buffer = unsigned int;


void processInput(GLFWwindow *window, Camera &camera) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.Orbit(1.f, glm::vec3(1.0f, 0.0f, 0.0f));
  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.Orbit(-1.f, glm::vec3(1.0f, 0.0f, 0.0f));
  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.Orbit(1.f, glm::vec3(0.0f, 1.0f, 0.0f));
  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.Orbit(-1.f, glm::vec3(0.0f, 1.0f, 0.0f));
  if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    camera.change_vert_offset(0.01);
  if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    camera.change_vert_offset(-0.01);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}


GLFWwindow *init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(Settings::SCR_WIDTH, Settings::SCR_HEIGHT, "Puma", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    throw std::runtime_error("Failed to init GLAD ");
  }

  glEnable(GL_DEPTH_TEST);
  return window;
}

std::tuple<buffer, buffer, buffer>
bindBuffers(const std::vector<objl::Vertex> &vertices,
            const std::vector<unsigned int> &indices) {
  buffer VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(objl::Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex),
                        (void *)offsetof(objl::Vertex, Position));
  glEnableVertexAttribArray(0);

  glBindVertexArray(0); // Unbind VAO
  return {VAO, VBO, EBO};
}

buffer bindTexture() {
  unsigned int texture1;
  // texture 1
  // ---------
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load image, create texture and generate mipmaps
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(
      true); // tell stb_image.h to flip loaded texture's on the y-axis.
  unsigned char *data = stbi_load(Paths::resources_metalic.c_str(), &width,
                                  &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  return texture1;
}


void preRender( GLFWwindow *window, const buffer & texture1, ModelParams &modelParams) {

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture1);
}

void updateShader(Shader &Shader, const ModelParams &modelParams, const Camera &Camera) {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    projection =
        glm::perspective(glm::radians(45.0f),
                         (float)Settings::SCR_WIDTH / (float)Settings::SCR_HEIGHT, 0.1f, 100.0f);
    // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    view = glm::lookAt(Camera.cameraPosition, Camera.getCameraTarget() ,
                       Camera.getCameraUp());

    Shader.setMat4("projection", projection);
    Shader.setMat4("view", view);
    Shader.setVec3("objectColor",
                   glm::vec3(1.0f, 0.5f,
                             0.31f)); // TODO: change color to somthing sensible


    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, modelParams.modelPosition);
    model = glm::rotate(model, glm::radians(modelParams.angle),
                        modelParams.RotationAxis);

    Shader.setMat4("model", model);
}

