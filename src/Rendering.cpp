#include "Rendering.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using buffer = unsigned int;

std::tuple<GLuint, GLuint, GLuint> load_vxo(std::string_view path,
                                            auto &vertices, auto &indices) {
  objl::Loader Loader;
  if (!Loader.LoadFile(path.data())) {
    throw std::runtime_error("Failed to load OBJ file.");
  }
  vertices = Loader.LoadedVertices;
  indices = Loader.LoadedIndices;
  return bindBuffers(vertices, indices);
}

Part::Part(std::string_view obj_path) {
    auto [VAO_, _, EBO_] =
        load_vxo(obj_path, vertices, indices);
    VAO.buffer = VAO_;
    EBO.buffer = EBO_;
  }

void Part::Render() const {
    glBindVertexArray(VAO.buffer);
    CheckForErrors(std::string("Binding VAO: ") +  std::to_string(VAO.buffer) );
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT,
                   0); // traiangle strip Works better ???? why ??
                       // GL_LINES_ADJACENCY also looks cool
                       // GL_LINES also looks cool
    CheckForErrors("Drawing elements");
  }

void processInput(GLFWwindow *window, Camera &camera) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.Orbit(1.f, glm::vec3(1.0f, 0.0f, 0.0f));
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.Orbit(-1.f, glm::vec3(1.0f, 0.0f, 0.0f));
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.Orbit(1.f, glm::vec3(0.0f, 1.0f, 0.0f));
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.Orbit(-1.f, glm::vec3(0.0f, 1.0f, 0.0f));
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    camera.change_vert_offset(0.01);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    camera.change_vert_offset(-0.01);
  if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    camera.change_radius(0.01);
  if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    camera.change_radius(-0.01);
}


void framebuffer_size_callback( GLFWwindow * window, int width, int height) {
  glViewport(0, 0, width, height);
}

GLFWwindow *init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(
      Settings::SCR_WIDTH, Settings::SCR_HEIGHT, "Puma", NULL, NULL);
  if (window == nullptr) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Failed to init GLAD ");
  }
  glEnable(GL_DEPTH_TEST);
  return window;
}

/* bindXXXX functions are ugly and I do realise that, but since they are
 * temporary I will make them sexier later on. They will have to be made more
 * robust anyway since rendering the whole robot as a big blob is not a good
 * idea, The whole robot is not a single object, but a collection of objects, so
 * different texutres and shaders will have to be used for different parts of
 * the robot but for now, for testing purposes, this is fine. I will change this
 * after the shaders are working and the robot is rendered correctly.
 * */

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
  CheckForErrors("bindBuffers");

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex),
                        (void *)offsetof(objl::Vertex, Position));
  glEnableVertexAttribArray(0);
  // normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex),
                        (void *)offsetof(objl::Vertex, Normal));
  glEnableVertexAttribArray(1);
  // Tex coordinate attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex),
                        (void *)offsetof(objl::Vertex, TextureCoordinate));
  glEnableVertexAttribArray(2);
  CheckForErrors("Attributes");

  glBindVertexArray(0); // Unbind VAO
  return {VAO, VBO, EBO};
}

[[deprecated("has some bugs do not use")]]
buffer bindTexture(std::string_view texturePath) {
    GLuint textureID;
  
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Configure the wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // or GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // or GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
    // Load the texture image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flipping the texture vertically if needed
    unsigned char *data = stbi_load(texturePath.data(), &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format{};
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture: " << texturePath << std::endl;
    }

    stbi_image_free(data);
    return textureID; // Return the texture ID
}

void preRender() {

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void updateShader(Shader const &Shader, const ModelParams &modelParams,
                  const Camera &Camera) {
  glm::mat4 view(1.0f);
  glm::mat4 projection(1.0f);
  projection = glm::perspective(
      glm::radians(45.0f),
      (float)Settings::SCR_WIDTH / (float)Settings::SCR_HEIGHT, 0.1f, 100.0f);
  // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  view = glm::lookAt(Camera.cameraPosition, Camera.getCameraTarget(),
                     Camera.getCameraUp());

  Shader.setMat4("projection", projection);
  Shader.setMat4("view", view);
  Shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
  Shader.setVec3("lightPos", Camera.cameraPosition); // I think it makes sense for max visibility
  Shader.setVec3("viewPos", Camera.cameraPosition);

  glm::mat4 model(1.0f);
  model = glm::translate(model, modelParams.modelPosition);
  model = glm::rotate(model, glm::radians(modelParams.angle),
                      modelParams.RotationAxis);

  Shader.setMat4("model", model);
}
