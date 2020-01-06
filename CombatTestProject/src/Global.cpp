#include "Global.h"
#include "GLResource.h"

using nativenamespace;

GLFWwindow* Native::Mouse::glfwWindow = NULL;
GLWindow* Native::Global::glWindow = NULL;
ShaderForLines Native::Global::lineShader;
ShaderForModels Native::Global::modelShader;
SkinnedMeshShader Native::Global::skeletonShader;
ShaderDeferedLighting Native::Global::deferedShader;
Camera Native::Global::gameCamera;
Viewport* Native::Viewport::window = NULL;
std::unordered_map<unsigned int, GLResource*> Native::GLResource::Resources;
