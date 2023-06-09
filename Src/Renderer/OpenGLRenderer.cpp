#include "AriaPCH.h"

#include "OpenGLRenderer.h"

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include <freetype/ftglyph.h>
#include FT_FREETYPE_H

#include <Utils/glDebug.h>

#include <learnopengl/filesystem.h>

#include "Test/TestTexture.h"
#include "Test/TestClearColor.h"
#include "Test/TestMono.h"

#include "Timer.h"

#define VectorSize(type, vector) sizeof(type) * vector.size()

namespace AriaRenderer
{
	OpenGLRenderer::~OpenGLRenderer()
	{
		delete currentTest;
		if (currentTest != testMenu)
		{
			delete testMenu;
		}
	}
	
	void OpenGLRenderer::setGLFWWindow(GLFWwindow* inWindow)
	{
		window = inWindow;
	
		glfwGetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
	}
	
	bool OpenGLRenderer::startup()
	{
		ARIA_CORE_TRACE("Engine Started.");
	
		bool success = initializeOpenGL();
		
		if (success)
		{
			setupDebugCallback();
	
			checkDSASupport();
	
			imGuiLayer.initImGui(window);
			changeOpenGLStates();
	
			
			prepareResources();
		}
	
		return success;
	}
	
	bool OpenGLRenderer::initializeOpenGL()
	{
		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			ARIA_CORE_CRITICAL("Failed to initialize GLAD.");
			return false;
		}
	
		ARIA_CORE_TRACE("Renderer Backend: OpenGL Core Profile 4.6 initialized.");
	
		//checkExtensions();
		checkComputeCapacity();
		testBed();
	
		setViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
		return true;
	}
	
	void OpenGLRenderer::setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
	{
		// make sure the viewport matches the new window dimensions; note that width and
		// height will be significantly larger than specified on retina displays.
		glViewport(x, y, width, height);
	
		SCREEN_WIDTH = width;
		SCREEN_HEIGHT = height;
		
		updateProjectionMatrix();
	
	}
	
	void OpenGLRenderer::updateProjectionMatrix()
	{
		projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);
	}
	
	void OpenGLRenderer::setupDebugCallback()
	{
		glDebugMessageControl(GL_DEBUG_SOURCE_API,
							  GL_DEBUG_TYPE_ERROR,
							  GL_DEBUG_SEVERITY_HIGH,
							  0, nullptr, GL_TRUE);
	
		glDebugMessageCallback(glDebugOutput, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
	
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
		// GL_DEBUG_OUTPUT_SYNCHRONOUS
		// If enabled, debug messages are produced synchronously by a debug context.
		// If disabled, debug messages may be produced asynchronously.In particular,
		// they may be delayed relative to the execution of GL commands, and the 
		// debug callback function may be called from a thread other than that in
		// which the commands are executed.See glDebugMessageCallback.
		// 如果不加这句，调试回调可能会从其他线程发起，从而无法得到异常调用堆栈
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
	
	bool OpenGLRenderer::checkDSASupport()
	{
		if (!GLAD_GL_ARB_direct_state_access)
		{
			/* see
			* https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_direct_state_access.txt
			* https://www.khronos.org/opengl/wiki/Direct_State_Access. This is the way.
			*/
			ARIA_CORE_CRITICAL("DSA not supported!");
			return false;
		}
	
		return true;
	}
	
	void OpenGLRenderer::checkExtensions()
	{
		int data;
		glGetIntegerv(GL_NUM_EXTENSIONS, &data);
	
		for (auto i = 0; i < data; i++)
		{
			auto name = (char*)glGetStringi(GL_EXTENSIONS, i);
			ARIA_CORE_TRACE(name);
		}
	}
	
	void OpenGLRenderer::checkComputeCapacity()
	{
		int workGroupCount[3];
	
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCount[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCount[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCount[2]);
	
		ARIA_CORE_TRACE("Max global(total) work group counts: x:{0} y:{1} z:{2}", workGroupCount[0], workGroupCount[1], workGroupCount[2]);
	
		int workGroupInvocations;
	
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInvocations);
	
		ARIA_CORE_TRACE("Max local work group invocations {0}", workGroupInvocations);
	}
	
	void OpenGLRenderer::changeOpenGLStates()
	{
		// configure global opengl state
		// -----------------------------
		//glDisable(GL_CULL_FACE);
		glEnable(GL_CULL_FACE);
		//glFrontFace(GL_CW);
		//glCullFace(GL_FRONT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
	
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f, -1.0f);
	}
	
	void OpenGLRenderer::prepareResources()
	{
		createTextures();
	
		GeometryGenerator::MeshData quadMesh = geometryGenerator.createQuad(50.0f, 50.0f, 25.0f);
		//GeometryGenerator::MeshData quadMesh = geometryGenerator.createQuad(2.0f, 2.0f);
	
		quad = std::make_shared<Model>();
	
		auto submesh = Mesh();
	
		quad->addMesh(submesh);
		createBuffers(quad->meshes[0], quadMesh);
	
		GeometryGenerator::MeshData screenQuadMesh = geometryGenerator.createQuad(2.0f, 2.0f);
		//GeometryGenerator::MeshData quadMesh = geometryGenerator.createQuad(2.0f, 2.0f);
	
		createBuffers(screenQuad, screenQuadMesh);
	
		GeometryGenerator::MeshData cubeMesh = geometryGenerator.createBox(4.0f, 4.0f, 4.0f, 0);
	
		createBuffers(mesh, cubeMesh);
	
		cube = std::make_shared<Model>();
	
		//model.load(MODEL_BASE + "rubber_duck/scene.gltf");
		cube->load(MODEL_BASE + "Box With Spaces/glTF/Box With Spaces.gltf");
	
		//createBuffers(cube.meshes[0]);
		cube->meshes[0].createBuffers();
	
		marry = std::make_shared<Model>();
	
		//marry->load(MODEL_BASE + "Box With Spaces/glTF/Box With Spaces.gltf");
		marry->load(MODEL_BASE + "Marry.gltf");
	
		for (uint32_t i = 0; i < marry->numMeshes; i++)
		{
			marry->meshes[i].createBuffers();
		}
	
		createColorFrameBufer();
		createDepthMapFrameBuffer();
		createDepthCubeMapFrameBuffer();
		setupShadowTransforms();
	
		//createRayTracingBuffers();
		//createRayMarchingBuffers();
		createShaders();
		createTextResources();
	
		setupPlane();
	
		testMenu = new TestMenu(currentTest);
		currentTest = testMenu;
	
		testMenu->RegisterTest<TestClearColor>("Clear Color");
		testMenu->RegisterTest<TestTexture>("Texture");
		testMenu->RegisterTest<TestMono>("Mono");

		lights[0].position = directionalShadowsLightPosition;
		lights[0].direction = -directionalShadowsLightPosition;
		lights[0].constant = 1.0f;
		lights[0].linear = 0.045f;
		lights[0].quadratic = 0.075f;

		lights[1].position = camera.Position;
		lights[1].direction = camera.Front;
		lights[1].cutOff = glm::cos(glm::radians(12.5f));
		lights[1].outerCutOff = glm::cos(glm::radians(17.5f));
		lights[1].constant = 1.0f;
		lights[1].linear = 0.09f;
		lights[1].quadratic = 0.032f;
	}
	
	void OpenGLRenderer::createBuffers(Mesh& mesh, const GeometryGenerator::MeshData& meshData)
	{
		// Setting up the triangle data
		// ----------------------------
		float testVertices[] = {
			-10.0f,  5.6f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-10.0f, -5.6f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 10.0f, -5.6f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			 10.0f,  5.6f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
		};
	
		//float normals[] = {
		//	1.0f, 0.0f, 0.0f,
		//	0.0f, 1.0f, 0.0f,
		//	0.0f, 0.0f, 1.0f
		//};
	
		//float texcoords[] = {
		//	0.0f, 0.0f,
		//	0.0f, 1.0f,
		//	1.0f, 1.0f,
		//	1.0f, 0.0f
		//};
	
		uint32_t testIndices[]{
			0, 1, 2,
			2, 3, 0
		};
	
		GLuint attributePosition = 0;
		GLuint attributeNormal = 1;
		GLuint attributeTexcoord = 2;
	
		/**************** VBO ***************/
		uint32_t positionVBO;	// hello colorful triangle vbo
		uint32_t normalVBO;		// hello colorful triangle vbo
		uint32_t texcoordVBO;	// hello colorful triangle vbo
		uint32_t allInOneVBO;
		//glGenBuffers(1, &VBO); // Way to go before openGl 4.5
		//glBindBuffer(GL_ARRAY_BUFFER, VBO); // Binding to openGl context was necessary
		// replaced with:
		// glCreateBuffers is the equivalent of glGenBuffers + glBindBuffer(the initialization part)
		//glCreateBuffers(1, &positionVBO);		//uses DSA. This is the way.
		//glCreateBuffers(1, &normalVBO);		//uses DSA. This is the way.
		//glCreateBuffers(1, &texcoordVBO);		//uses DSA. This is the way.
		//glCreateBuffers(1, &allInOneVBO);		//uses DSA. This is the way.
	
		//GeometryGenerator::MeshData meshData = geometryGenerator.createBox(5.0f, 5.0f, 5.0f, 0);
	
		//GeometryGenerator::MeshData meshData = geometryGenerator.createGrid(10.0f, 10.0f, 10, 10);
	
		//GeometryGenerator::MeshData meshData = geometryGenerator.createSphere(1.0f, 20, 20);
	
		mesh.vertices = geometryGenerator.toSimpleVertices(meshData.vertices);
	
		mesh.numVertices = static_cast<uint32_t>(mesh.vertices.size());
	
		mesh.indices = meshData.indices32;
		mesh.numIndices = static_cast<uint32_t>(mesh.indices.size());
	
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texcoords;
	
		geometryGenerator.buildSplitBuffers(mesh.vertices, positions, normals, texcoords);
	
		mesh.createBuffers();
	}
	
	void OpenGLRenderer::createBuffers(Mesh& mesh)
	{
		mesh.createBuffers();
	
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texcoords;
	
		geometryGenerator.buildSplitBuffers(mesh.vertices, positions, normals, texcoords);
	}
	
	void OpenGLRenderer::createRayTracingBuffers()
	{
		float width = static_cast<float>(SCREEN_WIDTH);
		float height = static_cast<float>(SCREEN_HEIGHT);
	
		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float vertices[] = 
		{
			// positions         
			 0.0f,  0.0f,	0.0f,  // bottom right
			 0.0f,  height, 0.0f,  // bottom left
			 width, height, 0.0f,   // top
	
			 width, height, 0.0f,
			 width, 0.0f,	0.0f,
			 0.0f,  0.0f,	0.0f
		};
	
		glCreateBuffers(1, &rayTracVBO);
	
		glNamedBufferStorage(rayTracVBO, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);
	
		glCreateVertexArrays(1, &rayTraceVAO);
	
		glVertexArrayVertexBuffer(rayTraceVAO, 0, rayTracVBO, 0, sizeof(float) * 3);
	
		GLuint attributePosition = 0;
	
		glEnableVertexArrayAttrib(rayTraceVAO, attributePosition);
	
		glVertexArrayAttribFormat(rayTraceVAO, attributePosition, 3, GL_FLOAT, false, 0);
	
		glVertexArrayAttribBinding(rayTraceVAO, attributePosition, 0);
	}
	
	void OpenGLRenderer::createRayMarchingBuffers()
	{
		float width = static_cast<float>(SCREEN_WIDTH);
		float height = static_cast<float>(SCREEN_HEIGHT);
	
		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float vertices[] =
		{
			// positions         
			 0.0f,  0.0f,	0.0f,  // bottom right
			 0.0f,  height, 0.0f,  // bottom left
			 width, height, 0.0f,   // top
	
			 width, height, 0.0f,
			 width, 0.0f,	0.0f,
			 0.0f,  0.0f,	0.0f
		};
	
		glCreateBuffers(1, &rayMarchingVBO);
	
		glNamedBufferStorage(rayMarchingVBO, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);
	
		glCreateVertexArrays(1, &rayMarchingVAO);
	
		glVertexArrayVertexBuffer(rayMarchingVAO, 0, rayMarchingVBO, 0, sizeof(float) * 3);
	
		GLuint attributePosition = 0;
	
		glEnableVertexArrayAttrib(rayMarchingVAO, attributePosition);
	
		glVertexArrayAttribFormat(rayMarchingVAO, attributePosition, 3, GL_FLOAT, false, 0);
	
		glVertexArrayAttribBinding(rayMarchingVAO, attributePosition, 0);
	}
	
	void OpenGLRenderer::createTextures()
	{
		albedoTexture = Texture::load(TEXTURE_BASE + "doge.jpg");
		albedoTexture.use(textureUnit);
	
		woodTexture = Texture::load(TEXTURE_BASE + "wood.png");
	
		skyboxDayTexture = Texture::loadCubemap(TEXTURE_BASE + "Skybox/day");
		skyboxDuskTexture = Texture::loadCubemap(TEXTURE_BASE + "Skybox/sunset");
		skyboxNightTexture = Texture::loadCubemap(TEXTURE_BASE + "Skybox/night");
	}
	
	void OpenGLRenderer::createShaders()
	{
		//shader.load(SHADER_BASE + "ShaderTriangle.vert", SHADER_BASE + "ShaderTriangle.frag");
		shader.loadSPV(SHADER_BASE + "ShaderTriangle.vert.spv", SHADER_BASE + "ShaderTriangle.frag.spv");
	
		shader.use();
	
		//shader.setInt("albedoTexture", textureUnit);
	
		int totalUnits;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &totalUnits);
	
		//textShader.load(SHADER_BASE + "Text.vert", SHADER_BASE + "Text.frag");
		textShader.loadSPV(SHADER_BASE + "Text.vert.spv", SHADER_BASE + "Text.frag.spv");
	
		//rayTraceShader.load(SHADER_BASE + "RayTrace.vert", SHADER_BASE + "RayTrace.frag");
		rayTraceShader.loadSPV(SHADER_BASE + "RayTrace.vert.spv", SHADER_BASE + "RayTrace.frag.spv");
	
		rayTraceShader.use();
	
		float width = static_cast<float>(SCREEN_WIDTH);
		float height = static_cast<float>(SCREEN_HEIGHT);
	
		// LearnOpenGL官方写法，这样实际上图片是上下颠倒的，而且顶点绕序是顺时针，开启glEnable(GL_CULL_FACE)时能正常渲染
		// 这么写是为了(0, 0)位于视口左上角
		glm::mat4 projection = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
	
		// 下面是正常的写法，(0, 0)位于视口左下角，需要相应修改顶点的位置，才能在开启glEnable(GL_CULL_FACE)时正常渲染
		//glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
	
		glm::mat4 model = glm::mat4(1.0f);
	
		rayTraceShader.setVec2i("viewport", { SCREEN_WIDTH, SCREEN_HEIGHT });
		rayTraceShader.setMat4("model", model);
		rayTraceShader.setMat4("projection", projection);
	
		//rayMarchingShader.load(SHADER_BASE + "RayMarching.vert", SHADER_BASE + "RayMarching.frag");
		rayMarchingShader.loadSPV(SHADER_BASE + "RayMarching.vert.spv", SHADER_BASE + "RayMarching.frag.spv");
	
		rayMarchingShader.use();
	
		rayMarchingShader.setVec2i("resolution", { SCREEN_WIDTH, SCREEN_HEIGHT });
		rayMarchingShader.setMat4("model", model);
		rayMarchingShader.setMat4("projection", projection);
	
		//screenQuadShader.load(SHADER_BASE + "ScreenQuad.vert", SHADER_BASE + "ScreenQuad.frag");
		screenQuadShader.loadSPV(SHADER_BASE + "ScreenQuad.vert.spv", SHADER_BASE + "ScreenQuad.frag.spv");
	
		//rayTracingShader.load(SHADER_BASE + "RayTracing.vert", SHADER_BASE + "RayTracing.frag");
		rayTracingShader.loadSPV(SHADER_BASE + "RayTracing.vert.spv", SHADER_BASE + "RayTracing.frag.spv");
	
		computeShader.initialize(glm::uvec2(SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8), glm::uvec2(SCREEN_WIDTH, SCREEN_HEIGHT));
		//computeShader.load(SHADER_BASE + "FloatArray.comp");
		computeShader.load(SHADER_BASE + "Image.comp");
		computeShader.prepareComputeResources();
	
		//renderDepthShader.load(SHADER_BASE + "RenderDepth.vert", SHADER_BASE + "RenderDepth.frag");
		renderDepthShader.loadSPV(SHADER_BASE + "RenderDepth.vert.spv", SHADER_BASE + "RenderDepth.frag.spv");
	
		debugDepthShader.loadSPV(SHADER_BASE + "DebugDepth.vert.spv", SHADER_BASE + "DebugDepth.frag.spv");
	
		pointShadowsDepthShader.loadSPV(SHADER_BASE + "PointShadowsDepth.vert.spv", SHADER_BASE + "PointShadowsDepth.frag.spv", SHADER_BASE + "PointShadowsDepth.geom.spv");
	
		pointShadowsShader.loadSPV(SHADER_BASE + "PointShadows.vert.spv", SHADER_BASE + "PointShadows.frag.spv");
	
		skyboxShader.loadSPV(SHADER_BASE + "Skybox.vert.spv", SHADER_BASE + "Skybox.frag.spv");
	
		reflectionShader.loadSPV(SHADER_BASE + "Reflectoin.vert.spv", SHADER_BASE + "Reflectoin.frag.spv");
	
		Shader::reset();
	}
	
	void OpenGLRenderer::createTextResources()
	{
		loadText(L"中文字体测试：0123456789°abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.+-", 0.0f, 0.0f);
	}
	
	void OpenGLRenderer::createColorFrameBufer()
	{
		renderTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	
	void OpenGLRenderer::createDepthMapFrameBuffer()
	{
		depthTexture.create(SHADOW_WIDTH, SHADOW_HEIGHT);
	}
	
	void OpenGLRenderer::createDepthCubeMapFrameBuffer()
	{
		depthCubeMap.create(SHADOW_WIDTH, SHADOW_HEIGHT);
	}
	
	void OpenGLRenderer::setupShadowTransforms()
	{
		float nearPlane = 1.0f;
		float farPlane = 25.0f;
		glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, nearPlane, farPlane);
		shadowTransforms.push_back(shadowProjection * glm::lookAt(pointShadowsLightPosition, pointShadowsLightPosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProjection * glm::lookAt(pointShadowsLightPosition, pointShadowsLightPosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProjection * glm::lookAt(pointShadowsLightPosition, pointShadowsLightPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProjection * glm::lookAt(pointShadowsLightPosition, pointShadowsLightPosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProjection * glm::lookAt(pointShadowsLightPosition, pointShadowsLightPosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProjection * glm::lookAt(pointShadowsLightPosition, pointShadowsLightPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	}
	
	uint32_t planeVAO = 0;
	
	void OpenGLRenderer::setupPlane()
	{
		// set up vertex data (and buffer(s)) and configure vertex attributes
	  // ------------------------------------------------------------------
		float planeVertices[] = {
			// positions            // normals         // texcoords
			 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
			-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
	
			 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
			 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
		};
		// plane VAO
		unsigned int planeVBO;
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
	}
	
	void OpenGLRenderer::update()
	{
		updateSkybox();
	}
	
	// renderCube() renders a 1x1 3D cube in NDC.
	// -------------------------------------------------
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	void renderCube()
	{
		// initialize (if necessary)
		if (cubeVAO == 0)
		{
			float vertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				// right face
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
				 // bottom face
				 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				 // top face
				 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
				  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
			};
			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	
	void OpenGLRenderer::run()
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//Wireframe rendering
		// render loop
		// -----------
		beginRender();
	
		//// 清除Render to texture帧缓冲
		//clearFrameBuffers(colorFBO, { 1.0f, 0.73f, 0.82f });
		//albedoTexture.use(textureUnit);
		//renderGeometry(quad);
	
		switch (scene)
		{
		case ESceneSelection::DirectionalShadow:
			// 清除Depth帧缓冲
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			depthTexture.clear();
	
			updateDepthMapShaderUniforms();
			renderDirectionalShadowMapScene(renderDepthShader, true);
	
			// 清除默认帧缓冲
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			clearFrameBuffers();
	
			//glBindTextureUnit(0, renderToTexture);
	
			depthTexture.use(1);
	
			updateDirectionalShadowMapUniforms(shader);
			renderDirectionalShadowMapScene(shader);
	
			drawSkybox(camera.GetViewMatrix(), projection);
	
			drawReflectionCube();
			break;
		case ESceneSelection::PointShadow:
			// 清除Depth帧缓冲
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			//clearDepthFrameBuffer(depthCubeMapFBO);
			depthCubeMap.clear();
	
			updateDepthCubeMapShaderUniforms();
			renderPointShadowMapScene(pointShadowsDepthShader, true);
	
			// 清除默认帧缓冲
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			clearFrameBuffers();
			//glBindTextureUnit(0, renderToTexture);
	
			depthCubeMap.use(1);
	
			updatePointShadowMapUniforms(pointShadowsShader);
			renderPointShadowMapScene(pointShadowsShader);
			break;
		case ESceneSelection::Test:
	
			break;
		case ESceneSelection::PathTracing:
			dispatchComputeShader();

			// In larger code you would prefer to put the barrier call closest to the code that actually samples the texture, 
			// so that you don't introduce any unnecessary waits
			computeShader.wait();
			glBindTextureUnit(textureUnit, computeShader.getTexture());

			screenQuadShader.use();
			screenQuadShader.setInt("albedo", textureUnit);

			renderGeometry(screenQuad);

			break;
		}
	
		if (debugDepth)
		{
			depthTexture.use(0);
	
			debugDepthShader.use();
	
			debugDepthShader.setFloat("nearPlane", orthoNearPlane);
			debugDepthShader.setFloat("farPlane", orthoFarPlane);
	
			renderGeometry(screenQuad);
		}
	
		//renderRayTracing(quad);
	
		//renderUI();
	
		endRender();
	}
	
	void OpenGLRenderer::renderUI()
	{
		drawText(textShader, L"FreeType中文测试：", 5.0f, SCREEN_HEIGHT - 45.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
	
		imGuiLayer.updateImGui();
		buildImGui();
		imGuiLayer.renderImGui();
	}
	
	void OpenGLRenderer::drawSkybox(const glm::mat4& inViewMatrix, const glm::mat4& inProjectionMatrix) 
	{
		glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.

		skyboxShader.use();
	
		glDepthFunc(GL_LEQUAL);
	
		glm::mat4 viewMatrix = inViewMatrix;
	
		// 消除平移部分，使天空盒和玩家一起移动(无限远的效果)
		viewMatrix[3][0] = 0.0f;
		viewMatrix[3][1] = 0.0f;
		viewMatrix[3][2] = 0.0f;
	
		glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(FrameTime * skyboxRotateRate), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
	
		skyboxShader.setMat4("model", model);
		skyboxShader.setMat4("view", viewMatrix);
		skyboxShader.setMat4("projection", inProjectionMatrix);
	
		renderCube();
	
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
	}
	
	void OpenGLRenderer::drawReflectionCube()
	{
		reflectionShader.use();
	
		auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	
		model = glm::scale(model, glm::vec3(0.5));
	
		reflectionShader.setMat4("model", model);
		reflectionShader.setMat4("view", camera.GetViewMatrix());
		reflectionShader.setMat4("projection", projection);
		reflectionShader.setVec3("viewPosition", camera.Position);
	
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(FrameTime * skyboxRotateRate), glm::vec3(0.0f, 1.0f, 0.0f));
	
		reflectionShader.setMat4("rotation", rotation);
	
		renderCube();
	}
	
	void OpenGLRenderer::renderScene(float deltaTime)
	{
		switch (renderMode)
		{
		case ERenderMode::Rasterizing:
			renderGeometry(mesh);
			break;
		case ERenderMode::RayTracing:
			renderRayTracing(mesh);
			break;
		default:
			break;
		}
	
		//renderRayMarching();
	
		frameCount++;
	}
	
	void OpenGLRenderer::renderDirectionalShadowMapScene(Shader& shader, bool renderDepth)
	{
		cube->position = { 2.0f, 0.0f, 1.0f };
		cube->scale = { 0.5f, 0.5f, 0.5f };
		cube->rotation = { 0.0f, 0.0f, 0.0f };
	
		woodTexture.use(0);
	
		updateObjectShaderUniform(shader, cube.get());
		renderCube();
		//renderGeometry(cube->meshes[0]);
	
		if (!renderDepth)
		{
			renderWireframeGeometry(cube->meshes[0]);
		}
	
		auto cube1 = cube;
		cube1->position = { 0.0f, 1.5f, 0.0 };
		cube1->scale = { 0.5f, 0.5f, 0.5f };
	
		updateObjectShaderUniform(shader, cube1.get());
		renderCube();
		//renderGeometry(cube1->meshes[0]);
	
		if (!renderDepth)
		{
			renderWireframeGeometry(cube1->meshes[0]);
		}
	
		auto cube2 = cube;
		cube2->position = { -1.0f, 0.0f, 2.0 };
		cube2->rotation = { 60.0f, 0.0f, 60.0f };
		cube2->scale = { 0.25f, 0.25f, 0.25f };
	
		updateObjectShaderUniform(shader, cube2.get());
		renderCube();
		//renderGeometry(cube2->meshes[0]);
	
		if (!renderDepth)
		{
			renderWireframeGeometry(cube2->meshes[0]);
		}
	
		quad->position = { 0.0f, -0.5f, 0.0f };
		quad->rotation = { -90.0f, 0.0f, 0.0f };
	
		updateObjectShaderUniform(shader, quad.get());
	
		renderGeometry(quad->meshes[0]);
	
		if (!renderDepth)
		{
			renderWireframeGeometry(quad->meshes[0]);
		}
	
		marry->position = { -3.0f, -0.5f, 0.0f };
	
		updateObjectShaderUniform(shader, marry.get());
	
		for (uint32_t i = 0; i < marry->numMeshes; i++)
		{
			renderGeometry(marry->meshes[i]);
		}
	}
	
	void OpenGLRenderer::renderPointShadowMapScene(Shader& shader, bool renderDepth /*= false*/)
	{
		// room cube
		auto room = cube;
	
		room->position = glm::vec3(0.0f);
		room->rotation = glm::vec3(0.0f);
		room->scale = glm::vec3(5.0f);
	
		woodTexture.use(0);
	
		updateObjectShaderUniform(shader, room.get());
		glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
		shader.setBool("reverseNormals", true); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
		renderCube();
		//renderGeometry(room->meshes[0]);
		shader.setBool("reverseNormals", false); // and of course disable it
		glEnable(GL_CULL_FACE);
	
		cube->position = { 4.0f, -3.5f, 0.0f };
		cube->rotation = glm::vec3(0.0f);
		cube->scale = { 0.5f, 0.5f, 0.5f };
	
		updateObjectShaderUniform(shader, cube.get());
		renderCube();
	
		////renderGeometry(cube->meshes[0]);
	
		if (!renderDepth)
		{
			renderWireframeGeometry(cube->meshes[0]);
		}
	
		auto cube1 = cube;
		cube1->position = { 2.0f, 3.0f, 1.0 };
		cube1->rotation = glm::vec3(0.0f);
		cube1->scale = { 0.75f, 0.75f, 0.75f };
	
		updateObjectShaderUniform(shader, cube1.get());
		renderCube();
	
		////renderGeometry(cube1->meshes[0]);
	
		if (!renderDepth)
		{
			renderWireframeGeometry(cube1->meshes[0]);
		}
	
		auto cube2 = cube;
		cube2->position = { -3.0f, -1.0f, 2.0 };
		cube2->rotation = { 0.0f, 0.0f, 0.0f };
		cube2->scale = { 0.5f, 0.5f, 0.5f };
	
		updateObjectShaderUniform(shader, cube2.get());
		renderCube();
	
		////renderGeometry(cube2->meshes[0]);
	
		if (!renderDepth)
		{
			renderWireframeGeometry(cube2->meshes[0]);
		}
	
		auto cube3 = cube;
	
		cube3->position = { -1.5f, 1.0f, 1.5f };
		cube3->rotation = { 0.0f, 0.0f, 0.0f };
		cube3->scale = { 0.5f, 0.5f, 0.5f };
	
		updateObjectShaderUniform(shader, cube3.get());
		renderCube();
		//renderGeometry(quad->meshes[0]);
	
		if (!renderDepth)
		{
			renderWireframeGeometry(cube3->meshes[0]);
		}
	
		auto cube4 = cube;
	
		cube4->position = { -1.5f, 2.0f, -3.0 };
		cube4->rotation = { 60.0f, 0.0f, 60.0f };
		cube4->scale = { 0.75f, 0.75f, 0.75f };
	
		updateObjectShaderUniform(shader, cube3.get());
		renderCube();
		//renderGeometry(quad->meshes[0]);
	
		if (!renderDepth)
		{
			renderWireframeGeometry(cube3->meshes[0]);
		}
	}
	
	void OpenGLRenderer::renderGeometry(const Mesh& mesh)
	{
		mesh.use();
	
		if (mesh.numTexture > 0)
		{
			mesh.textures[0].use();
		}
	
		glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
	
		// In larger code you would prefer to put the barrier call closest to the code that actually samples the texture, 
		// so that you don't introduce any unnecessary waits
		//computeShader.wait();
		//glBindTextureUnit(textureUnit, computeShader.getTexture());
	
		//screenQuadShader.use();
		//screenQuadShader.setInt("albedo", textureUnit);
	}
	
	void OpenGLRenderer::renderWireframeGeometry(const Mesh& mesh)
	{
		mesh.VAO.bind();
	
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shader.setBool("isWireframe", true);
		glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
		shader.setBool("isWireframe", false);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	void OpenGLRenderer::updateObjectShaderUniform(Shader& shader, Model* object)
	{
		shader.use();
	
		auto S = glm::scale(glm::mat4(1.0f), object->scale);
	
		auto R = glm::mat4(1.0f);
	
		R = glm::rotate(R, glm::radians(object->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		R = glm::rotate(R, glm::radians(object->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		R = glm::rotate(R, glm::radians(object->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	
		auto T = glm::translate(glm::mat4(1.0f), object->position);
	
		auto model = T * R * S;
	
		shader.setMat4("model", model);
	}
	
	void OpenGLRenderer::updateDirectionalShadowMapUniforms(Shader& shader)
	{
		shader.use();
	
		// view/projection transformations
		glm::mat4 view = camera.GetViewMatrix();
	
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setBool("useTexture", useTexture);
		shader.setBool("renderDepth", renderDepth);
		shader.setVec3("viewPosition", camera.Position);
	
		auto lightView = glm::lookAt(directionalShadowsLightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, orthoNearPlane, orthoFarPlane);
		auto lightSpaceMatrix = lightProjection * lightView;
	
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		shader.setVec3("lights[0].position", lights[0].position);
		shader.setVec3("lights[0].direction", lights[0].direction);
		shader.setFloat("lights[0].constant", lights[0].constant);
		shader.setFloat("lights[0].linear", lights[0].linear);
		shader.setFloat("lights[0].quadratic", lights[0].quadratic);

		shader.setVec3("lightPosition", directionalShadowsLightPosition);
		shader.setVec3("lights[1].position", camera.Position);
		shader.setVec3("lights[1].direction", camera.Front);
		shader.setFloat("lights[1].cutOff", lights[1].cutOff);
		shader.setFloat("lights[1].outerCutOff", lights[1].outerCutOff);
		shader.setFloat("lights[1].constant", lights[1].constant);
		shader.setFloat("lights[1].linear", lights[1].linear);
		shader.setFloat("lights[1].quadratic", lights[1].quadratic);
	
		GLuint blockIndex = glGetUniformBlockIndex(shader.ID, "UniformBuffer");
	}
	
	void OpenGLRenderer::updatePointShadowMapUniforms(Shader& shader)
	{
		shader.use();
	
		// view/projection transformations
		glm::mat4 view = camera.GetViewMatrix();
	
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setBool("useTexture", useTexture);
		shader.setBool("renderDepth", renderDepth);
		shader.setVec3("viewPosition", camera.Position);
		shader.setVec3("lightPosition", pointShadowsLightPosition);
		shader.setFloat("farPlane", perpsectiveFarPlane);
	}
	
	void OpenGLRenderer::updateDepthMapShaderUniforms(bool perspectiveProjection)
	{
		renderDepthShader.use();
	
		auto lightView = glm::lookAt(directionalShadowsLightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, orthoNearPlane, orthoFarPlane);
	
		if (perspectiveProjection)
		{
			lightProjection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT), perpsectiveNearPlane, perpsectiveFarPlane);
		}
	
		auto lightSpaceMatrix = lightProjection * lightView;
	
		renderDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		renderDepthShader.setVec3("lightPosition", directionalShadowsLightPosition);
	}
	
	void OpenGLRenderer::updateDepthCubeMapShaderUniforms()
	{
		pointShadowsDepthShader.use();
	
		pointShadowsDepthShader.setFloat("farPlane", perpsectiveFarPlane);
		pointShadowsDepthShader.setVec3("lightPosition", pointShadowsLightPosition);
	
		for (unsigned int i = 0; i < 6; ++i)
		{
			pointShadowsDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		}
	}
	
	void OpenGLRenderer::updateSkybox()
	{
		skyboxRotateRate += 1.0f;
	
		timeOfDay += FrameTime;
	
		skyboxShader.use();
	
		float alpha = 0.0f;
	
		if (timeOfDay < 10.0f) 
		{
			skyboxDayTexture.use(0);
		}
	
		if ((10.0f < timeOfDay) && (timeOfDay < 20.0f))
		{
			skyboxDayTexture.use(0);
			skyboxDuskTexture.use(1);
	
			alpha = (timeOfDay - 10.0f) / 10.0f;
		}
	
		if ((20.0f < timeOfDay) && (timeOfDay < 30.0f))
		{
			skyboxDuskTexture.use(0);
			skyboxNightTexture.use(1);
	
			alpha = (timeOfDay - 20.0f) / 10.0f;
		}
	
		if ((30.0f < timeOfDay) && (timeOfDay < 40.0f))
		{
			skyboxNightTexture.use(0);
			skyboxDayTexture.use(1);
	
			alpha = (timeOfDay - 30.0f) / 10.0f;
		}
	
		if (timeOfDay > 40.0f) 
		{
			timeOfDay = 0.0f;
		}
	
		skyboxShader.setFloat("alpha", alpha);
	
		reflectionShader.use();
	
		reflectionShader.setFloat("alpha", alpha);
	}
	
	void OpenGLRenderer::renderRayTracing(const Mesh& mesh)
	{
		//rayTraceShader.use();
	
		//glBindVertexArray(rayTraceVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
	
		rayTracingShader.use();
	
		rayTracingShader.setVec2i("resolution", glm::ivec2(SCREEN_WIDTH, SCREEN_HEIGHT));
		rayTracingShader.setFloat("time", static_cast<float>(glfwGetTime()));
	
		mesh.VAO.bind();
	
		glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
	}
	
	void OpenGLRenderer::renderRayMarching()
	{
		rayMarchingShader.use();
	
		rayMarchingShader.setFloat("time", static_cast<float>(glfwGetTime()));
	
		glBindVertexArray(rayMarchingVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	
	void OpenGLRenderer::dispatchComputeShader()
	{
		computeShader.use();
		computeShader.dispatch();
	
		// 很慢
		//auto computeData = computeShader.getComputeData();
	
		//for (auto data : computeData) {
		//	std::cout << data << " ";
		//}
		//std::cout << std::endl;
	
		auto i = 0;
	}
	
	void OpenGLRenderer::buildImGui()
	{
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
	
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;
	
			ImGui::Begin(reinterpret_cast<const char*>(u8"测试"));                          // Create a window called "Hello, world!" and append into it.
			//ImGui::SetWindowSize(ImVec2(400.0f, 400.0f));
			//ImGui::SetWindowPos(ImVec2(SCR_WIDTH - 400.0f, 0.0f));
			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &showAnotherWindow);
	
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clearCcolor); // Edit 3 floats representing a color
	
			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);
	
			ImGui::Text("OpenGLRenderer average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	
			ImGui::Text(reinterpret_cast<const char*>(u8"显示中文试试"));
	
			if (ImGui::Button(reinterpret_cast<const char*>(u8"设置")))
				showAnotherWindow = true;
	
			//static float f0 = 0.001f;
			//ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");
	
			ImGui::End();
		}
	
		// 3. Show another simple window.
		if (showAnotherWindow)
		{
			ImGui::Begin("Another Window", &showAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				showAnotherWindow = false;
			ImGui::End();
		}
	
		if (currentTest)
		{
			currentTest->onUpdate(FrameTime);
			currentTest->onRender(shader);
	
			ImGui::Begin("Test");
			if (currentTest != testMenu && ImGui::Button("<-"))
			{
				delete currentTest;
				currentTest = testMenu;
			}
			currentTest->onImGuiRender();
			ImGui::End();
		}
	}
	
	void OpenGLRenderer::imGuiShutdown()
	{
		imGuiLayer.imGuiShutdown();
	}
	
	void OpenGLRenderer::loadText(const wchar_t* text, GLfloat x, GLfloat y)
	{
		std::string fontPath = FileSystem::getPath(FONT_BASE + "YaHei.Consolas.1.12.ttf");
	
		//初始化FreeType库
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			ARIA_CORE_CRITICAL("FreeType初始化失败");
		}
	
		FT_Face face;
		if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
		{
			ARIA_CORE_CRITICAL("字体加载失败");
		}
	
		//定义字体大小
		FT_Set_Pixel_Sizes(face, 0, 48);
	
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制
		auto lew_w = wcslen(text);
	
		for (size_t i = 0; i < lew_w; i++)
		{
			// 加载字符的字形 
			if (FT_Load_Char(face, text[i], FT_LOAD_RENDER))
			{
				ARIA_CORE_CRITICAL("字形加载失败");
	
				continue;
			}
			// 字形大小
			int fontWidth = face->glyph->bitmap.width;
			int fontRows = face->glyph->bitmap.rows;
	
			// 从基准线到字形左部/顶部的偏移值
			int fontLeft = face->glyph->bitmap_left;
			int fontTop = face->glyph->bitmap_top;
	
			// 原点距下一个字形原点的距离
			int fontX = face->glyph->advance.x;
			unsigned char* data = face->glyph->bitmap.buffer;
	
			// 生成纹理
			// glCreateTextures is the equivalent of glGenTextures + glBindTexture(for initialization).
			GLuint charactertexture;
			glCreateTextures(GL_TEXTURE_2D, 1, &charactertexture);
	
			// 根据上面的文档，第二个参数levels必须大于等于1，和下面glTextureSubImage2D的levels(mipmap levels)参数含义不同
			// 第三个参数internalformat Specifies the sized internal format to be used to store texture image data.
			// 详情见https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
			glTextureStorage2D(charactertexture, 1, GL_R8, fontWidth, fontRows);
	
			// target
			//	 Specifies the target to which the texture is bound for glTexSubImage2D.
			//	 Must be GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 
			//   GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 
			//   GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, or GL_TEXTURE_1D_ARRAY.
	
			// texture
			//	 Specifies the texture object name for glTextureSubImage2D.
			//   The effective target of texture must be one of the valid target values above.
	
			// level
			//   Specifies the level - of - detail number.Level 0 is the base image level.
			//   Level n is the nth mipmap reduction image.
	
			// xoffset
			//   Specifies a texel offset in the x direction within the texture array.
	
			// yoffset
			//   Specifies a texel offset in the y direction within the texture array.
	
			// width
			//   Specifies the width of the texture subimage.
	
			// height
			//   Specifies the height of the texture subimage.
	
			// format
			//   Specifies the format of the pixel data.The following symbolic values are accepted :
			//   GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, and GL_STENCIL_INDEX.
	
			// type
			//   Specifies the data type of the pixel data.The following symbolic values are accepted : 
			//   GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT,
			//   GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, 
			//   GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV,
			//   GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.
			glTextureSubImage2D(charactertexture, 0, 0, 0, fontWidth, fontRows, GL_RED, GL_UNSIGNED_BYTE, data);
	
			// 设置纹理选项
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
			Character character = 
			{
				charactertexture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
			};
	
			unicodeCharacters.insert(std::pair<wchar_t, Character>(text[i], character));
		}
	
		// configure VAO/VBO for texture quads
		// -----------------------------------
		glCreateBuffers(1, &unicodeVBO);
	
		glNamedBufferStorage(unicodeVBO, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_STORAGE_BIT);
	
		//glBindVertexArray(unicodeVAO);
		glCreateVertexArrays(1, &unicodeVAO);
	
		GLuint vaoBindingPoint = 0;
	
		glVertexArrayVertexBuffer(
			unicodeVAO,                 // vao to bind
			vaoBindingPoint,       // Could be 1, 2... if there were several vbo to source.
			unicodeVBO,            // VBO to bound at "vaoBindingPoint".
			0,                      // offset of the first element in the buffer hctVBO.
			4 * sizeof(float));     // stride == 3 position floats + 3 color floats.
	
		GLuint attributePosition = 0;
		GLuint attributeTexcoord = 1;
	
		glEnableVertexArrayAttrib(unicodeVAO, attributePosition);// Need to precise vao, as there is no context binding in DSA style
		glEnableVertexArrayAttrib(unicodeVAO, attributeTexcoord);// Meaning no current vao is bound to the opengl context.
	
		//glVertexAttribPointer(attribPos, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		//glVertexAttribPointer(attribCol, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)( 3*sizeof(float) ));
		//  replaced with:
		glVertexArrayAttribFormat(unicodeVAO, attributePosition, 2, GL_FLOAT, false, 0);// Need to precise vao, as there is no context binding in DSA
		glVertexArrayAttribFormat(unicodeVAO, attributeTexcoord, 2, GL_FLOAT, false, 8);//https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribFormat.xhtml
	
		//Explicit binding of an attribute to a vao binding point
		glVertexArrayAttribBinding(unicodeVAO, attributePosition, vaoBindingPoint);
		glVertexArrayAttribBinding(unicodeVAO, attributeTexcoord, vaoBindingPoint);
	}
	
	void OpenGLRenderer::drawText(Shader& shader, const std::wstring& text, float x, float y, float scale, glm::vec3 color)
	{
		shader.use();
	
		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH), 0.0f, static_cast<float>(SCREEN_HEIGHT));
	
		shader.setMat4("projection", projection);
		shader.setVec3("textColor", color);
	
		glBindVertexArray(unicodeVAO);
	
		std::wstring::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = unicodeCharacters[*c];
	
			float xpos = x + ch.bearing.x * scale;
			float ypos = y - (ch.size.y - ch.bearing.y) * scale;
	
			float w = ch.size.x * scale;
			float h = ch.size.y * scale;
	
			// update VBO for each character
			float vertices[6][4] = 
			{
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
	
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};
	
			// render glyph texture over quad
			glBindTextureUnit(0, ch.textureID);
	
			glNamedBufferSubData(unicodeVBO, 0, sizeof(vertices), vertices);
	
			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
	
			x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}
	}
	
	void OpenGLRenderer::beginRender()
	{
		//clearColorFrameBuffer();
		//clearDepthFrameBuffer();
	
		GLfloat color[]{ 0.4f, 0.6f, 0.9f, 1.0f };
		glClearNamedFramebufferfv(0, GL_COLOR, 0, color);
	
		GLfloat depth[]{ 1.0f, 1.0f, 1.0f, 1.0f };
		glClearNamedFramebufferfv(0, GL_DEPTH, 0, depth);
	}
	
	void OpenGLRenderer::clearColorFrameBuffer(uint32 frameBuffer, const std::array<GLfloat, 4>& clearColor)
	{
		// render
		// ------
		// So the DSA version looks like this:
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glClearNamedFramebufferfv(frameBuffer, GL_COLOR, 0, clearColor.data());
	}
	
	void OpenGLRenderer::clearDepthFrameBuffer(uint32 frameBuffer, const std::array<GLfloat, 4>& clearValue)
	{
		// So the DSA version looks like this:
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glClearNamedFramebufferfv(frameBuffer, GL_DEPTH, 0, clearValue.data());
	}
	
	void OpenGLRenderer::clearFrameBuffers(uint32 frameBuffer, const std::array<GLfloat, 4>& clearColor, const std::array<GLfloat, 4>& clearValue)
	{
		clearColorFrameBuffer(frameBuffer, clearColor);
		clearDepthFrameBuffer(frameBuffer, clearValue);
	}
	
	void OpenGLRenderer::endRender()
	{
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		//glfwSwapBuffers(window);
	}
	
	void OpenGLRenderer::testBed()
	{
		float aspect = 16.0f / 9.0f;
		float viewportHeight = 2.0;;
		float viewportWidth = viewportHeight * aspect;
		float focalLength = 1.0;
	
		glm::vec3 origin{0.0f};
	
		glm::vec3 horizontal = glm::vec3(viewportWidth, 0.0, 0.0);
		glm::vec3 vertical = glm::vec3(0.0, viewportHeight, 0.0);
	
		glm::vec3 lowerLeftCorner = origin - horizontal * 0.5f - vertical * 0.5f - glm::vec3(0.0, 0.0, focalLength);
	
		glm::ivec2 resolution{SCREEN_WIDTH, SCREEN_HEIGHT};
		
		glm::ivec2 texelCoord;
	
		for (texelCoord.y = 0; texelCoord.y < SCREEN_HEIGHT; texelCoord.y++)
		{
			for (texelCoord.x = 0; texelCoord.x < SCREEN_WIDTH; texelCoord.x++)
			{
				float x = (float(texelCoord.x * 2.0 - resolution.x) / resolution.x);
				float y = (float(texelCoord.y * 2.0 - resolution.y) / resolution.y);
	
				//x = (float(texelCoord.x) / resolution.x);
				//y = (float(texelCoord.y) / resolution.y);
	
				glm::vec3 direction = glm::vec3(viewportWidth * x, viewportHeight * y, -1.0f);
	
				//direction = lowerLeftCorner + x * horizontal + y * vertical - origin;
	
				//fmt::print("{0}, {1}, {2}\n", direction.x, direction.y, direction.z);
			}
		}
	}
}