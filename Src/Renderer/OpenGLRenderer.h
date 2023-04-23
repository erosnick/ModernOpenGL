#include <iostream>
#include <map>

#include "Camera.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <Utils/GeometryGenerator.h>

#include <array>

#include "ComputeShader.h"
#include "Texture.h"
#include "Model.h"
#include "ImGui/ImGuiLayer.h"

#include "RenderTexture.h"
#include "DepthTexture.h"
#include "DepthCubeMap.h"

#include "Test/Test.h"

#include "Light.h"

using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using int32 = std::int32_t;

const float FrameTime = 0.0166667f;

namespace AriaRenderer
{
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
	
	enum class ESceneSelection
	{
		Test,
		DirectionalShadow,
		PointShadow
	};
	
	class OpenGLRenderer
	{ 
	public:
		OpenGLRenderer() {}
		~OpenGLRenderer();
	
		void setGLFWWindow(GLFWwindow* inWindow);
	
		bool startup();
	
		bool initializeOpenGL();
	
		void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);
	
		void updateProjectionMatrix();
	
		void setupDebugCallback();
	
		bool checkDSASupport();
	
		void checkExtensions();
	
		void checkComputeCapacity();
	
		void changeOpenGLStates();
	
		void prepareResources();
	
		void createBuffers(Mesh& mesh, const GeometryGenerator::MeshData& meshData);
		void createBuffers(Mesh& mesh);
	
		void createRayTracingBuffers();
	
		void createRayMarchingBuffers();
	
		void createTextures();
	
		void createShaders();
	
		void createTextResources();
	
		void createColorFrameBufer();
		void createDepthMapFrameBuffer();
		void createDepthCubeMapFrameBuffer();
	
		void setupShadowTransforms();
	
		void setupPlane();
	
		void update();
	
		void run();
	
		void renderUI();
	
		void drawSkybox(const glm::mat4& inViewMatrix, const glm::mat4& inProjectionMatrix);
	
		void drawReflectionCube();
	
		void renderScene(float deltaTime);
	
		void renderDirectionalShadowMapScene(Shader& shader, bool renderDepth = false);
		void renderPointShadowMapScene(Shader& shader, bool renderDepth = false);
	
		void renderGeometry(const Mesh& mesh);
		void renderWireframeGeometry(const Mesh& mesh);
	
		void updateObjectShaderUniform(Shader& shader, Model* object);
		void updateDirectionalShadowMapUniforms(Shader& shader);
		void updatePointShadowMapUniforms(Shader& shader);
		void updateDepthMapShaderUniforms(bool perspectiveProjection = false);
		void updateDepthCubeMapShaderUniforms();
	
		void updateSkybox();
	
		void renderRayTracing(const Mesh& mesh);
	
		void renderRayMarching();
	
		void dispatchComputeShader();
	
		// ImGui
		void buildImGui();
		void imGuiShutdown();
	
		void loadText(const wchar_t* text, GLfloat x, GLfloat y);
	
		void drawText(Shader& shader, const std::wstring& text, float x, float y, float scale, glm::vec3 color);
	
		void beginRender();
	
		void clearColorFrameBuffer(uint32 frameBuffer = 0, const std::array<GLfloat, 4>& clearColor = { 0.4f, 0.6f, 0.9f, 1.0 });
		void clearDepthFrameBuffer(uint32 frameBuffer = 0, const std::array<GLfloat, 4>& clearValue = { 1.0f, 1.0f, 1.0f, 1.0 });
	
		void clearFrameBuffers(uint32 frameBuffer = 0, const std::array<GLfloat, 4>& clearColor = { 0.4f, 0.6f, 0.9f, 1.0 }, const std::array<GLfloat, 4>& clearValue = { 1.0f, 1.0f, 1.0f, 1.0 });
	
		void endRender();
	
		GLenum getRenderMode() { return polygonMode; }
	
		void toggleTexture() { useTexture = !useTexture; }
	
		void toggleRenderDepth() { renderDepth = !renderDepth; }
		void toggleDebugDepth() { debugDepth = !debugDepth; }
	
		void testBed();
		
		void setRenderMode(ERenderMode inRenderMode) { renderMode = inRenderMode; }
		ERenderMode getRenderMode() const { return renderMode; }
	
		Camera& getCamera() { return camera; }
	
	public:
		ESceneSelection scene = ESceneSelection::DirectionalShadow;
	private:
		const std::string FONT_BASE = "Assets/Fonts/";
		const std::string SHADER_BASE = "Assets/Shaders/";
		const std::string MODEL_BASE = "Assets/Models/";
		const std::string TEXTURE_BASE = "Assets/Textures/";
	
		std::map<int32, Character> characters;
		std::map<int32, Character> unicodeCharacters;
	
		uint32 unicodeVAO;
		uint32 unicodeVBO;
	
		uint32 textureUnit = 0;
	
		uint32 rayTraceVAO;
		uint32 rayTracVBO;
	
		uint32 rayMarchingVAO;
		uint32 rayMarchingVBO;
	
		DepthTexture depthTexture;
		DepthCubeMap depthCubeMap;
	
		RenderTexture renderTexture;
	
		// Our state
		bool showDemoWindow = true;
		bool showAnotherWindow = false;
		ImVec4 clearCcolor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	
		bool useTexture = true;
		bool renderDepth = false;
		bool isWireframe = false;
		bool debugDepth = false;
	
		const float orthoNearPlane = 1.0f;
		const float orthoFarPlane = 7.5f;
	
		const float perpsectiveNearPlane = 1.0f;
		const float perpsectiveFarPlane = 25.0f;
	
		glm::vec3 directionalShadowsLightPosition{ -2.0f, 4.0f, -1.0f };
		glm::vec3 pointShadowsLightPosition{ 0.0f, 0.0f, 0.0f };
	
		std::vector<glm::ivec2> resolutions{ { 1920, 1080 }, { 2560, 1440 }, { 3840, 2160 } };
	
		// settings
		int32 SCREEN_WIDTH = resolutions[0].x;
		int32 SCREEN_HEIGHT = resolutions[0].y;
	
		const int32 SHADOW_WIDTH = 2048;
		const int32 SHADOW_HEIGHT = 2048;
	
		Camera camera{ glm::vec3(0.0f, 0.0f, 3.0f) };
	
		glm::mat4 projection;
	
		Shader shader;
		Shader textShader;
		Shader rayTraceShader;
		Shader rayMarchingShader;
		Shader screenQuadShader;
		Shader rayTracingShader;
		Shader renderDepthShader;
		Shader debugDepthShader;
	
		Shader pointShadowsDepthShader;
		Shader pointShadowsShader;
	
		Shader skyboxShader;
		Shader reflectionShader;
	
		Texture albedoTexture;
		Texture woodTexture;
		Texture skyboxDayTexture;
		Texture skyboxDuskTexture;
		Texture skyboxNightTexture;
	
		ComputeShader computeShader{ glm::uvec2(SCREEN_WIDTH, SCREEN_HEIGHT) };
	
		GLFWwindow* window;
	
		float frameTime = 0.0f;
	
		float rotateAngle = 0.0f;
	
		float skyboxRotateRate = 1.0f;
	
		float timeOfDay = 0.0f;
	
		int32 frameCount = 0;
	
		GLenum polygonMode = GL_FILL;
	
		ERenderMode renderMode = ERenderMode::Rasterizing;
	
		GeometryGenerator geometryGenerator;
	
		std::shared_ptr<Model> cube;
	
		Mesh mesh;
		std::shared_ptr<Model> quad;
		Mesh screenQuad;
	
		std::shared_ptr<Model> marry;
	
		std::vector<std::shared_ptr<Model>> models;
	
		std::vector<glm::mat4> shadowTransforms;
	
		ImGuiLayer imGuiLayer;
	
		Test* currentTest = nullptr;
		TestMenu* testMenu = nullptr;

		Light lights[2];
	};
}