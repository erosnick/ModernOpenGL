#include <iostream>
#include <map>

#include <learnopengl/camera.h>
#include <learnopengl/shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <Utils/GeometryGenerator.h>

#include <array>

#include "ComputeShader.h"
#include "Texture.h"

using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using int32 = std::int32_t;

// Holds all state information relevant to a character as loaded using FreeType
struct Character
{
	uint32 textureID; // ID handle of the glyph texture
	glm::ivec2   size;      // Size of glyph
	glm::ivec2   bearing;   // Offset from baseline to left/top of glyph
	uint32 advance;   // Horizontal offset to advance to next glyph
};

enum class ERenderMode
{
	Rasterizing,
	RayTracing
};

struct Mesh
{
	uint32 indicesCount() const { return static_cast<uint32>(indices.size()); }

	std::vector<GeometryGenerator::SimpleVertex> vertices;
	std::vector<uint32> indices;

	uint32 VAO;
	uint32 VBO;
	uint32 EBO;
};

class Application
{
public:
	// Forward declarations
	static void framebufferSizeCallback(GLFWwindow* window, int32 width, int32 height);
	static void keyCallback(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	void processInput(GLFWwindow* window);

	bool startup();

	bool initializeOpenGL();

	void setupDebugCallback();

	bool checkDSASupport();

	void checkExtensions();

	void checkComputeCapacity();

	void changeOpenGLStates();

	void prepareResources();

	void createBuffers(Mesh& mesh, const GeometryGenerator::MeshData& meshData);

	void createRayTracingBuffers();

	void createRayMarchingBuffers();

	void createTextures();

	void createShaders();

	void createTextResources();

	void createColorFrameBufer();
	void createDepthFrameBuffer();

	void run();

	void renderUI();

	void renderScene(float deltaTime);

	void renderGeometry(const Mesh& mesh);

	void updateSceneShaderUniforms(const glm::vec3& translation);
	void updateDepthShaderUniforms();

	void renderRayTracing(const Mesh& mesh);

	void renderRayMarching();

	void dispatchComputeShader();

	// ImGui
	void initImGui(GLFWwindow* window);
	void buildImGui();

	void updateImGui();
	void renderImGui();

	void ImGuiShutdown();

	void loadText(const wchar_t* text, GLfloat x, GLfloat y);

	void drawText(Shader& shader, const std::wstring& text, float x, float y, float scale, glm::vec3 color);

	void updateFPSCounter(GLFWwindow* window);

	void beginRender();

	void clearColorFrameBuffer(uint32 frameBuffer = 0, const std::array<GLfloat, 4>& clearColor = { 0.4f, 0.6f, 0.9f, 1.0 });
	void clearDepthFrameBuffer(uint32 frameBuffer = 0, const std::array<GLfloat, 4>& clearValue = { 1.0f, 1.0f, 1.0f, 1.0 });

	void clearFrameBuffers(uint32 frameBuffer = 0, const std::array<GLfloat, 4>& clearColor = { 0.4f, 0.6f, 0.9f, 1.0 }, const std::array<GLfloat, 4>& clearValue = { 1.0f, 1.0f, 1.0f, 1.0 });

	void endRender();

	GLenum getRenderMode() { return polygonMode; }

	void toggleTexture() { useTexture = !useTexture; }

	void toggleRenderDepth() { renderDepth = !renderDepth; }

	void testBed();
	
	void setRenderMode(ERenderMode inRenderMode) { renderMode = inRenderMode; }
	ERenderMode getRenderMode() const { return renderMode; }

	void processMouseScroll(float yOffset);

private:

	const std::string SHADER_BASE = "./Assets/Shaders/";
	const std::string MODEL_BASE = "./Assets/Models/";
	const std::string TEXTURE_BASE = "./Assets/Textures/";

	std::map<int32, Character> characters;
	std::map<int32, Character> unicodeCharacters;

	uint32 unicodeVAO;
	uint32 unicodeVBO;

	uint32 textureUnit = 0;

	uint32 rayTraceVAO;
	uint32 rayTracVBO;

	uint32 rayMarchingVAO;
	uint32 rayMarchingVBO;

	uint32 colorFBO;
	uint32 depthFBO;

	uint32 renderToTexture;
	uint32 depthTexture;

	// Our state
	bool showDemoWindow = true;
	bool showAnotherWindow = false;
	ImVec4 clearCcolor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool useTexture = true;
	bool renderDepth = false;

	std::vector<glm::ivec2> resolutions{ {1280, 720}, {2560, 1440}, {3840, 2160} };

	// settings
	const int32 SCREEN_WIDTH = resolutions[0].x;
	const int32 SCREEN_HEIGHT = resolutions[0].y;

	const int32 SHADOW_WIDTH = 1024;
	const int32 SHADOW_HEIGHT = 1024;

	Camera camera{ glm::vec3(0.0f, 0.0f, 10.0f) };

	Shader shader;
	Shader textShader;
	Shader rayTraceShader;
	Shader rayMarchingShader;
	Shader screenQuadShader;
	Shader rayTracingShader;
	Shader renderDepthShader;

	Texture albedoTexture;

	ComputeShader computeShader{ glm::uvec2(SCREEN_WIDTH, SCREEN_HEIGHT) };

	GLFWwindow* window;

	float frameTime = 0.0f;

	float fixFrameTime = 0.016667f;

	float rotateAngle = 0.0f;

	int32 frameCount = 0;

	GLenum polygonMode = GL_FILL;

	ERenderMode renderMode = ERenderMode::Rasterizing;

	GeometryGenerator geometryGenerator;

	Mesh mesh;
	Mesh quad;
};