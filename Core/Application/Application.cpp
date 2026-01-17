#include "Application.h"
#include "../Utils/image_helper.h"
#include "../BlockDatabase.h"
#include "../Player/Player.h"

void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data)
{
	char* _source;
	char* _type;
	char* _severity;

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		_source = (char*)"API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		_source = (char*)"WINDOW SYSTEM";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		_source = (char*)"SHADER COMPILER";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		_source = (char*)"THIRD PARTY";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		_source = (char*)"APPLICATION";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		_source = (char*)"UNKNOWN";
		break;

	default:
		_source = (char*)"UNKNOWN";
		break;
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		_type = (char*)"ERROR";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		_type = (char*)"DEPRECATED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		_type = (char*)"UDEFINED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		_type = (char*)"PORTABILITY";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
		_type = (char*)"PERFORMANCE";
		return;
		break;

	case GL_DEBUG_TYPE_OTHER:
		_type = (char*)"OTHER";
		break;

	case GL_DEBUG_TYPE_MARKER:
		_type = (char*)"MARKER";
		break;

	default:
		_type = (char*)"UNKNOWN";
		break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		_severity = (char*)"HIGH";
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		_severity = (char*)"MEDIUM";
		break;

	case GL_DEBUG_SEVERITY_LOW:
		_severity = (char*)"LOW";
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		_severity = (char*)"NOTIFICATION";
		break;

	default:
		_severity = (char*)"UNKNOWN";
		break;
	}

	printf("%d: %s of %s severity, raised from %s: %s\n",
		id, _type, _severity, _source, msg);
}

namespace Omnia
{
	static const constexpr bool ShouldInitializeImgui = true;
	const float default_player_speed = 0.05f;
	const float default_player_sensitivity = 0.25f;
	const float default_fov = 110.0f;
	float ex_PlayerSpeed = default_player_speed;
	float ex_PlayerSensitivity = default_player_sensitivity;
	float ex_FOV = default_fov;

	Application OmniaApplication;

	static void glfwErrorCallback(int error, const char* description)
	{
		fprintf(stderr, "GLFW ERROR!   %d: %s\n", error, description);
	}

	bool FilenameIsValid(const std::string& filepath)
	{
		FILE* file;

		file = fopen(filepath.c_str(), "w+");
		
		if (!file)
		{
			return false;
		}

		else
		{
			fclose(file);
			std::filesystem::remove(filepath);
			return true;
		}
	}

	Application::Application() : m_GameState(GameState::MenuState), m_OrthagonalCamera(0.0f, (float)DEFAULT_WINDOW_X, 0.0f, (float)DEFAULT_WINDOW_Y)
	{
		const char* glsl_version = static_cast<const char*>("#version 130");

		glfwSetErrorCallback(glfwErrorCallback);

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

		GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);

		m_Window = glfwCreateWindow(mode->width, mode->height, "OMNIA V0.01 Beta", NULL, NULL);

		if (m_Window == NULL)
		{
			Logger::LogToConsole("Failed to create window!");
			glfwTerminate();
		}

		glfwMakeContextCurrent(m_Window);

		glfwSwapInterval(1);

		glewInit();

		char* renderer = (char*)glGetString(GL_RENDERER);
		char* vendor = (char*)glGetString(GL_VENDOR);
		char* version = (char*)glGetString(GL_VERSION);

		Logger::LogOpenGLInit(renderer, vendor, version);

		// Lock the cursor to the window
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		EventSystem::InitEventSystem(m_Window, &m_EventQueue);

		// Setting up imgui context
		if (ShouldInitializeImgui)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::StyleColorsDark();

			ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
			ImGui_ImplOpenGL3_Init(glsl_version);

			io.Fonts->AddFontDefault();

			// Custom Fonts
			Fonts::Custom::customFont_MainTitle = io.Fonts->AddFontFromMemoryTTF(exo2bold, sizeof(exo2bold), 40.0f);
			Fonts::Custom::customFont_MainDesc = io.Fonts->AddFontFromMemoryTTF(exo2light, sizeof(exo2light), 24.0f);

			// Default Fonts
			Fonts::Exo2::thin = io.Fonts->AddFontFromMemoryTTF(exo2thin, sizeof(exo2thin), 16.0f);
			Fonts::Exo2::thinitalic = io.Fonts->AddFontFromMemoryTTF(exo2thinitalic, sizeof(exo2thinitalic), 16.0f);
			Fonts::Exo2::extralight = io.Fonts->AddFontFromMemoryTTF(exo2extralight, sizeof(exo2extralight), 16.0f);
			Fonts::Exo2::extralightitalic = io.Fonts->AddFontFromMemoryTTF(exo2extralightitalic, sizeof(exo2extralightitalic), 16.0f);
			Fonts::Exo2::light = io.Fonts->AddFontFromMemoryTTF(exo2light, sizeof(exo2light), 16.0f);
			Fonts::Exo2::lightitalic = io.Fonts->AddFontFromMemoryTTF(exo2lightitalic, sizeof(exo2lightitalic), 16.0f);
			Fonts::Exo2::bold = io.Fonts->AddFontFromMemoryTTF(exo2bold, sizeof(exo2bold), 16.0f);

			io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
			io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
			io.IniFilename = nullptr;
		}

		// Turn on depth 
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_CursorLocked = true;

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		GUI::InitUISystem(m_Window);

		// Initialize random menu world
		// Initialize random menu world
		srand(time(NULL));
		int w, h;
		glfwGetFramebufferSize(m_Window, &w, &h);
		m_World = new World(rand() % 100000, glm::vec2(w, h), "MenuWorld", WorldGenerationType::Generation_Normal);
		m_World->p_Player->p_FreeFly = true; 
		
		// Force chunk generation to find ground
		m_World->OnUpdate(m_Window, false);

		float spawn_y = 100.0f;
		if (m_World->ChunkExistsInMap(0, 0))
		{
			for (int y = CHUNK_SIZE_Y - 1; y > 0; y--)
			{
				if (m_World->GetBlockTypeFromPosition(glm::vec3(0, y, 0)) != BlockType::Air)
				{
					spawn_y = (float)y + 3.0f; // +2 blocks above ground
					break;
				}
			}
		}

		m_World->p_Player->p_Position = glm::vec3(0, spawn_y, 0); 
		m_World->p_Player->p_Camera.SetPosition(glm::vec3(0, spawn_y, 0) + glm::vec3(0.0f, Player::EyeOffsetY, 0.0f));

		m_Renderer2D = new Renderer2D;

		// Queue a window resize event to properly scale the cameras (according to the window dimensions)
		EventSystem::Event e;
		e.type = EventSystem::EventTypes::WindowResize;
		m_EventQueue.push_back(e);
	}

	Application::~Application()
	{
		if (ShouldInitializeImgui)
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}

		GUI::CloseUIContext();
		//Clouds::DestroyClouds();
		glfwDestroyWindow(m_Window);

		delete m_Renderer2D;
		delete m_World;
	}

	void Application::OnUpdate()
	{
		// Calculate delta time
		float currentFrameTime = (float)glfwGetTime();
		m_DeltaTime = currentFrameTime - m_LastFrameTime;
		m_LastFrameTime = currentFrameTime;

		if (ShouldInitializeImgui)
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		// Poll the events
		PollEvents();

		// Enable depth testing and blending
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Clear the depth and color bit buffer

		glClearColor(0.44f, 0.78f, 0.88f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		if (m_World)
		{
			// Render the world in ALL states
			// Only show crosshair if playing or paused (optional)
			bool show_cross = (m_GameState == GameState::PlayingState && !m_InventoryOpen); 
			m_World->RenderWorld(show_cross);

			// Determine if we should update player and use deltaTime
			bool should_update_player = (m_GameState == GameState::PlayingState && !m_InventoryOpen);
			bool is_paused = (m_GameState == GameState::PauseState);
			float effective_deltaTime = (m_InventoryOpen || is_paused) ? 0.0f : m_DeltaTime;
			
			// Always update world (generates chunks, lights, etc.)
			// but only process player input if playing and inventory is not open
			if (m_GameState == GameState::PlayingState) 
			{
				if (!m_InventoryOpen)
					glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
				
			m_World->OnUpdate(m_Window, effective_deltaTime, should_update_player);

			if (m_GameState != GameState::PlayingState)
			{
				// Rotate camera slowly in menu / pause / other states (frame-independent)
				float currentYaw = m_World->p_Player->p_Camera.GetYaw();
				m_World->p_Player->p_Camera.SetYaw(currentYaw + 10.0f * m_DeltaTime);
				m_World->p_Player->p_Camera.Refresh(); 
			}
		}

		if (ShouldInitializeImgui)
		{
			OnImGuiRender();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		GUI::RenderUI(glfwGetTime(), 0);

		// Render imgui and swap the buffers after rendering ui components, world etc..
		GLClasses::DisplayFrameRate(m_Window, "OMNIA V0.01 Beta");
		glfwSwapBuffers(m_Window);
		glfwPollEvents();

		// Obtain the debug info every second
		
		if (std::remainderf(glfwGetTime(), 2.0f) <= 0.0f)
		{
			PlatformSpecific::GetProcDebugInfo(m_ProcDebugInfo);
		}
	}

	void Application::OnImGuiRender()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		static int renderdistance = 6;
		static bool first_run = true;
		int w, h;
		glfwGetFramebufferSize(m_Window, &w, &h);
		static char input[64];
		static int seed = 0;
		static int world_type = 0;
		static bool show_invalid_name_error = false;
		static double error_display_time = 0.0;
		std::vector<std::string> Saves;
		if (m_GameState == GameState::PlayingState && m_World)
		{
			bool open = true;
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground;
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);
			if (ImGui::Begin("##Omnia-InGame", &open, window_flags))
			{
				float stats_width = 200.0f;
				float stats_start_x = 15.0f;
				float stats_start_y = h - 200.0f; 
				ImGui::SetCursorPos(ImVec2(stats_start_x, stats_start_y));
				auto DrawImGuiBar = [&](float val, float max_val, const char* label, ImVec4 color) {
					ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
					char overlay[32];
					sprintf(overlay, "%s %.0f/%.0f", label, val, max_val);
					ImGui::ProgressBar(val / max_val, ImVec2(stats_width, 18), overlay);
					ImGui::PopStyleColor(2);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
					ImGui::SetCursorPosX(stats_start_x);
				};
				DrawImGuiBar(m_World->p_Player->p_Health, 100.0f, "Health", ImVec4(0.9f, 0.3f, 0.23f, 1.0f)); // Red
				DrawImGuiBar(m_World->p_Player->p_Armor, 100.0f, "Armor", ImVec4(0.7f, 0.75f, 0.78f, 1.0f));  // Silver
				DrawImGuiBar(m_World->p_Player->p_Hunger, 100.0f, "Hunger", ImVec4(0.95f, 0.61f, 0.07f, 1.0f)); // Orange
				DrawImGuiBar(m_World->p_Player->p_Temperature, 100.0f, "Air", ImVec4(0.2f, 0.6f, 0.86f, 1.0f));   // Blue
				
				// Hotbar rendering - dinamik pozisyon
				if (!m_InventoryOpen)
				{
					// Normal pozisyon (sol alt)
					float hotbar_width = 9 * 50.0f + 10 * 5.0f;
					float start_x = 15.0f;
					float start_y = h - 60.0f;
					RenderHotbar(start_x, start_y, hotbar_width);
				}
				if (m_ShowDebugInfo) {
					ImGui::PushFont(Fonts::Exo2::bold);
					ImGui::SetCursorPos(ImVec2(15, 15));
					stringstream ss;
					BlockType current_block = static_cast<BlockType>(m_World->p_Player->p_CurrentHeldBlock);
					const glm::vec3& pos = m_World->p_Player->p_Position;
					ss << "VSync: " << m_VSync << "\n";
					ss << "Current Held Block: " << BlockDatabase::GetBlockName(current_block).c_str() << "\n";
					ss << "Player Position X: " << pos.x << " | Y: " << pos.y << " | Z: " << pos.z << "\n";
					ss << "Collision: " << m_World->p_Player->p_IsColliding << "\n";
					ss << "Freefly (Noclip): " << m_World->p_Player->p_FreeFly << "\n";
					ss << "Chunk Amount: " << m_World->m_ChunkCount << "\n";
					ss << "Loaded Chunks: " << m_World->p_ChunksRendered << "\n";
					ss << "Sun Position: " << m_World->GetSunPositionY() << "\n";
					ss << "Total CPU Used: " << m_ProcDebugInfo.cpu_usage << "\n";
					ss << "Total Memory: " << m_ProcDebugInfo.total_mem << "  /  " << m_ProcDebugInfo.total_mem_used << "\n";
					ss << "Total Virtual Memory: " << m_ProcDebugInfo.total_vm << "  /  " << m_ProcDebugInfo.total_vm_used << "\n";
					ss << "CPU Usage: " << m_ProcDebugInfo.cpu_usage << "\n";
					ImGui::Text(ss.str().c_str());
					ImGui::PopFont();
				}
				ImGui::End();
			}
		}
		if (first_run)
		{
			memset(&input, '\0', 64);
			omnialogo = LoadTextureFromMemory_GL(omnia_logo, sizeof(omnia_logo), &logoWidth, &logoHeight);
			first_run = false;
		}
		if (m_GameState == GameState::MenuState)
		{
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			bool open = true;
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground;
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);
			style.Colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.15f, 1.00f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.05f, 0.05f, 0.25f, 1.00f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.05f, 0.05f, 0.25f, 1.00f);
			style.WindowBorderSize = 0.0f;
			style.WindowPadding = ImVec2(0, 0);
			if (ImGui::Begin("##Omnia-Main", &open, window_flags))
			{
				if (omnialogo) {
					ImGui::SetCursorPos(ImVec2(-10, 680));
					ImGui::Image(omnialogo, ImVec2(90, 90));
					ImGui::SetCursorPos(ImVec2(70, 700));
					ImGui::PushFont(Fonts::Custom::customFont_MainTitle); ImGui::Text(menuTitle.c_str()); ImGui::PopFont();
					ImGui::SetCursorPos(ImVec2(15, 750));
					ImGui::PushFont(Fonts::Custom::customFont_MainDesc); ImGui::Text(menuDesc.c_str()); ImGui::PopFont();
				}
				else {
					ImGui::SetCursorPos(ImVec2(15, 700));
					ImGui::PushFont(Fonts::Custom::customFont_MainTitle); ImGui::Text(menuTitle.c_str()); ImGui::PopFont();
					ImGui::SetCursorPos(ImVec2(15, 740));
					ImGui::PushFont(Fonts::Custom::customFont_MainDesc); ImGui::Text(menuDesc.c_str()); ImGui::PopFont();
				}
				ImGui::PushFont(Fonts::Exo2::light);
				ImGui::SetCursorPos(ImVec2(15, 870));
				if (ImGui::Button("Play", ImVec2(150, 40))) m_GameState = GameState::WorldSelectState;
				ImGui::SetCursorPos(ImVec2(15, 920));
				if (ImGui::Button("Info", ImVec2(150, 40))) m_GameState = GameState::HelpState;
				ImGui::SetCursorPos(ImVec2(15, 970));
				if (ImGui::Button("Settings", ImVec2(150, 40))) m_GameState = GameState::SettingsState;
				ImGui::SetCursorPos(ImVec2(15, 1020));
				if (ImGui::Button("Quit", ImVec2(150, 40))) glfwSetWindowShouldClose(m_Window, true);
				ImGui::PopFont();
				ImGui::End();
			}
		}
		static GameState prev_settings_state;
		if (m_GameState != GameState::SettingsState) prev_settings_state = m_GameState;
		if (m_GameState == GameState::SettingsState)
		{
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);;
			bool open = true;
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;
			ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.4f);
			style.WindowBorderSize = 0.0f;
			style.WindowPadding = ImVec2(0, 0);
			if (ImGui::Begin("##Omnia-Settings", &open, window_flags))
			{
				ImGui::PushFont(Fonts::Custom::customFont_MainDesc);
				ImGui::SetCursorPos(ImVec2(450, 440));
				ImGui::SetNextItemWidth(950.0f);
				ImGui::SliderFloat("Player Speed", &ex_PlayerSpeed, 0.01, 0.18f);
				ImGui::SetCursorPos(ImVec2(450, 480));
				ImGui::SetNextItemWidth(950.0f);
				ImGui::SliderFloat("Sensitivity", &ex_PlayerSensitivity, 0.01, 1.5f);
				ImGui::SetCursorPos(ImVec2(450, 520));
				ImGui::SetNextItemWidth(950.0f);
				ImGui::SliderInt("Render Distance", &renderdistance, 2, 16);
				ImGui::SetCursorPos(ImVec2(450, 560));
				ImGui::SetNextItemWidth(950.0f);
				ImGui::SliderFloat("Field of View", &ex_FOV, 60.0f, 110.0f);
				ImGui::SetCursorPos(ImVec2(450, 600));
				if (ImGui::Button("Apply"))
				{
					m_GameState = prev_settings_state;
					m_World->SetRenderDistance(renderdistance);
					// Only apply FOV to actual game worlds, not menu world
					if (m_World && m_World->p_Player && m_World->GetName() != "MenuWorld")
					{
						m_World->p_Player->p_Camera.SetFov(ex_FOV);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
				{
					ex_PlayerSpeed = default_player_speed;
					ex_PlayerSensitivity = default_player_sensitivity;
					ex_FOV = default_fov;
					// Only reset FOV for actual game worlds, not menu world
					if (m_World && m_World->p_Player && m_World->GetName() != "MenuWorld")
					{
						m_World->p_Player->p_Camera.SetFov(ex_FOV);
					}
				}
				ImGui::PopFont();
				ImGui::End();
			}
		}
		else if (m_GameState == GameState::PauseState)
		{
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2((float)w, (float)h), IM_COL32(0, 0, 0, 100));
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			bool open = true;
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.4f);
			style.WindowBorderSize = 0.0f;
			style.WindowPadding = ImVec2(0, 0);
			if (ImGui::Begin("##Omnia-Pause", &open, window_flags))
			{
				ImGui::PushFont(Fonts::Exo2::light);
				ImGui::SetCursorPos(ImVec2(w / 2 - 100, 450)); if (ImGui::Button("Resume", ImVec2(200, 40))) m_GameState = GameState::PlayingState;
				ImGui::SetCursorPos(ImVec2(w / 2 - 100, 500)); if (ImGui::Button("Settings", ImVec2(200, 40))) m_GameState = GameState::SettingsState;
				ImGui::SetCursorPos(ImVec2(w / 2 - 100, 550)); if (ImGui::Button("Save and Quit", ImVec2(200, 40)))
				{
					WorldFileHandler::SaveWorld(m_World->GetName(), m_World);
					m_GameState = GameState::MenuState;
					delete m_World;
					m_World = new World(rand() % 100000, glm::vec2(w, h), "MenuWorld", WorldGenerationType::Generation_Normal);
					m_World->p_Player->p_FreeFly = true;
					m_World->OnUpdate(m_Window, false);
					float spawn_y = 100.0f;
					if (m_World->ChunkExistsInMap(0, 0))
					{
						for (int y = CHUNK_SIZE_Y - 1; y > 0; y--)
						{
							if (m_World->GetBlockTypeFromPosition(glm::vec3(0, y, 0)) != BlockType::Air)
							{
								spawn_y = (float)y + 3.0f;
								break;
							}
						}
					}
					m_World->p_Player->p_Position = glm::vec3(0, spawn_y, 0); 
					m_World->p_Player->p_Camera.SetPosition(glm::vec3(0, spawn_y, 0) + glm::vec3(0.0f, Player::EyeOffsetY, 0.0f));
				}
				ImGui::PopFont();
				ImGui::End();
			}
		}
		else if (m_GameState == GameState::WorldSelectState)
		{
			if (std::filesystem::exists("Saves/"))
			{
				for (auto entry : std::filesystem::directory_iterator("Saves/"))
				{
					Saves.push_back(entry.path().filename().string());
				}
			}
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			bool open = true;
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.4f);
			style.WindowBorderSize = 0.0f;
			style.WindowPadding = ImVec2(0, 0);
			if (ImGui::Begin("##Omnia-WorldSelect", &open, window_flags))
			{
				ImGui::PushFont(Fonts::Custom::customFont_MainDesc);
				ImGui::SetCursorPos(ImVec2(450, 230));
				style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.7f);
				if (ImGui::BeginChild("##Omnia-WorldList", ImVec2(1000, 600), false)) {
					for (int i = 0; i < Saves.size(); i++)
					{
						if (ImGui::Button(Saves.at(i).c_str(), ImVec2(1000, 75)))
						{
							m_World = WorldFileHandler::LoadWorld(Saves.at(i));
							m_GameState = GameState::PlayingState;
							EventSystem::Event e;
							e.type = EventSystem::EventTypes::WindowResize;
							m_EventQueue.push_back(e);
							break;
						}
					}
					ImGui::EndChild();
				}
				ImGui::SetCursorPos(ImVec2(450, 850));
				if (ImGui::Button("Create")) m_GameState = GameState::WorldCreateState;
				ImGui::SameLine();
				if (ImGui::Button("Back")) m_GameState = GameState::MenuState;
				ImGui::PopFont();
				ImGui::End();
			}
		}
		else if (m_GameState == GameState::WorldCreateState)
		{
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			bool open = true;
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.4f);
			style.WindowBorderSize = 0.0f;
			style.WindowPadding = ImVec2(0, 0);
			if (ImGui::Begin("##Omnia-CreateWorld", &open, window_flags))
			{
				ImGui::PushFont(Fonts::Custom::customFont_MainDesc);
				ImGui::SetCursorPos(ImVec2(450, 440));
				ImGui::SetNextItemWidth(950.0f); ImGui::InputText("World Name", input, sizeof(input));
				ImGui::SetCursorPos(ImVec2(450, 480));
				ImGui::SetNextItemWidth(950.0f); ImGui::InputInt("Seed", &seed);
				const char* world_types[] = { "Normal", "Islands", "Hilly", "Flat", "Flat+" };
				ImGui::SetCursorPos(ImVec2(450, 520));
				ImGui::SetNextItemWidth(950.0f); ImGui::Combo("World Type", &world_type, world_types, IM_ARRAYSIZE(world_types));
				ImGui::SetCursorPos(ImVec2(450, 570));
				if (ImGui::Button("Create World"))
				{
					bool isValid = FilenameIsValid(input);
					if (isValid)
					{
						m_World = new World(seed, glm::vec2(w, h), input, static_cast<WorldGenerationType>(world_type));
						m_GameState = GameState::PlayingState;
						memset(input, '\0', 64);
						show_invalid_name_error = false;
					}
					else {
						show_invalid_name_error = true;
						error_display_time = glfwGetTime();
					}
				}
				if (show_invalid_name_error)
				{
					double current_time = glfwGetTime();
					if (current_time - error_display_time < 5.0)
					{
						ImGui::SameLine();
						ImGui::TextColored(ImColor(255, 0, 0, 255), "Invalid World Name!");
					}
					else
					{
						show_invalid_name_error = false;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Back")) m_GameState = GameState::WorldSelectState;
				ImGui::PopFont();
				ImGui::End();
			}
		}
		else if (m_GameState == GameState::HelpState)
		{
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			bool open = true;
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.4f);
			style.WindowBorderSize = 0.0f;
			style.WindowPadding = ImVec2(0, 0);
			if (ImGui::Begin("##Omnia-Help", 0, window_flags))
			{
				ImGui::PushFont(Fonts::Custom::customFont_MainDesc);
				ImGui::SetCursorPos(ImVec2(450, 340)); ImGui::Text("A Voxel-Based Sandbox Game");
				ImGui::SetCursorPos(ImVec2(450, 360)); ImGui::Text("By Saiyan0x (timucinozdemir0@gmail.com)");
				ImGui::SetCursorPos(ImVec2(450, 380)); ImGui::Text("Discord: saiyan0x");
				ImGui::SetCursorPos(ImVec2(450, 400)); ImGui::Text("If you like this project. Please consider starring it on GitHub");
				ImGui::SetCursorPos(ImVec2(450, 420)); ImGui::Text("All art and resources are not mine. Credits go to their respective owners");
				ImGui::SetCursorPos(ImVec2(450, 440)); ImGui::Text("All Credits Going to Original Creator: https://github.com/swr06/Minecraft");
				ImGui::SetCursorPos(ImVec2(450, 480)); ImGui::Text("--- Instructions ---");
				ImGui::SetCursorPos(ImVec2(450, 500)); ImGui::Text("Toggle Debug Context: Ctrl + Shift + F12");
				ImGui::SetCursorPos(ImVec2(450, 520)); ImGui::Text("Movement: W S A D");
				ImGui::SetCursorPos(ImVec2(450, 540)); ImGui::Text("Space: Fly Up");
				ImGui::SetCursorPos(ImVec2(450, 560)); ImGui::Text("Left Shift: Fly Down");
				ImGui::SetCursorPos(ImVec2(450, 580)); ImGui::Text("Right Mouse Button: Place Block");
				ImGui::SetCursorPos(ImVec2(450, 600)); ImGui::Text("Left Mouse Button: Break Block");
				ImGui::SetCursorPos(ImVec2(450, 620)); ImGui::Text("Change Current Block: (Q) Previous / (E) Next");
				ImGui::SetCursorPos(ImVec2(450, 640)); ImGui::Text("Toggle VSync: V");
				ImGui::SetCursorPos(ImVec2(450, 660)); ImGui::Text("Toggle Collision Mode (Noclip): F");
				ImGui::SetCursorPos(ImVec2(450, 680)); ImGui::Text("Reset Sun Position: G");
				ImGui::SetCursorPos(ImVec2(450, 720)); if (ImGui::Button("Back")) m_GameState = GameState::MenuState;
				ImGui::PopFont();
				ImGui::End();
			}
		}
		if (m_InventoryOpen && m_GameState == GameState::PlayingState && m_World)
		{
			// Arka planı karart
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2((float)w, (float)h), IM_COL32(0, 0, 0, 150));

			// Envanter paneli - ekranın ortasında
			int slots_per_row = 10;
			float slot_size = 60.0f;
			float slot_spacing = 10.0f;
			float inv_width = slots_per_row * slot_size + (slots_per_row + 3) * slot_spacing; // 9 slot * 60px + 10 boşluk * 10px
			float inv_height = 600.0f; // Sabit yükseklik, scrollable içerik
			float inv_x = (w - inv_width) / 2.0f;
			float inv_y = (h - inv_height) / 2.0f;

			// Hotbar konumu (inventory açıkken ayrı overlay window'da render edilecek)
			float hotbar_width = 9 * 50.0f + 10 * 5.0f;
			float hotbar_x = inv_x + (inv_width - hotbar_width) / 2.0f;
			float hotbar_y = inv_y + inv_height + 20.0f;

			ImGuiWindowFlags inv_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowPos(ImVec2(inv_x, inv_y), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(inv_width, inv_height), ImGuiCond_Always);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.7f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
			
			if (ImGui::Begin("##Inventory", nullptr, inv_flags))
			{
				// Tüm blokları topla (Air ve UnknownBlockType hariç)
				static std::vector<BlockType> all_blocks;
				static bool blocks_initialized = false;
				
				if (!blocks_initialized)
				{
					blocks_initialized = true;
					for (int i = 0; i <= (int)BlockType::Air; i++)
					{
						BlockType block = (BlockType)i;
						if (block != BlockType::Air && block != BlockType::UnknownBlockType)
						{
							all_blocks.push_back(block);
						}
					}
				}

				ImTextureID atlas_tex = (ImTextureID)(intptr_t)m_World->GetRenderer()->GetAtlasTexture()->GetTextureID();
				float atlas_w = (float)m_World->GetRenderer()->GetAtlasTexture()->GetWidth();
				float atlas_h = (float)m_World->GetRenderer()->GetAtlasTexture()->GetHeight();
				
				int total_rows = (int)std::ceil((float)all_blocks.size() / (float)slots_per_row);
				
				for (int row = 0; row < total_rows; row++)
				{
					// Satır başında boşluk bırak (ortalamak için)
					int blocks_in_row = std::min(slots_per_row, (int)all_blocks.size() - row * slots_per_row);
					if (blocks_in_row < slots_per_row)
					{
						// Eksik slotlar varsa ortalamak için boşluk ekle
						float row_width = blocks_in_row * slot_size + (blocks_in_row - 1) * slot_spacing;
						float padding = (inv_width - 20.0f - row_width) / 2.0f; // 20.0f = window padding
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding);
					}
					
					for (int col = 0; col < slots_per_row; col++)
					{
						int index = row * slots_per_row + col;
						if (index >= (int)all_blocks.size())
							break;
						
						BlockType block = all_blocks[index];
						ImGui::PushID(index);
						
						// Slot arka planı için bir group/frame
						ImGui::BeginGroup();
						
						// Slot arka planı için button frame
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.8f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.9f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
						
						// Blok texture'ı
						const std::array<uint16_t, 8>& uv_raw = BlockDatabase::GetBlockTexture(block, BlockFaceType::front);
						float x1 = uv_raw[2] / atlas_w;
						float y1 = uv_raw[3] / atlas_h;
						float x2 = uv_raw[0] / atlas_w;
						float y2 = uv_raw[5] / atlas_h;
						ImVec2 uv0 = ImVec2(x1, y2);
						ImVec2 uv1 = ImVec2(x2, y1);
						
						// ImageButton kullan (tıklanabilir ve görsel)
						ImGui::ImageButton(atlas_tex, ImVec2(slot_size, slot_size), uv0, uv1, 0, ImVec4(0.2f, 0.2f, 0.2f, 0.8f), ImVec4(1,1,1,1));
						
						// Drag başlatma (envanter'den - tıklama veya sürükleme)
						if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !m_IsDragging)
						{
							m_IsDragging = true;
							m_DraggedBlock = block;
						}
						
						// Sürükleme ile de drag başlat
						if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !m_IsDragging)
						{
							m_IsDragging = true;
							m_DraggedBlock = block;
						}
						
						// Drop (envanter'e geri - swap için)
						if (m_IsDragging && ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
						{
							BlockType temp = block;
							all_blocks[index] = m_DraggedBlock;
							m_DraggedBlock = temp;
							if (m_DraggedBlock == BlockType::Air)
								m_IsDragging = false;
						}
						
						ImGui::PopStyleColor(3);
						
						// Tooltip
						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text("%s", BlockDatabase::GetBlockName(block).c_str());
							ImGui::EndTooltip();
						}
						
						ImGui::EndGroup();
						
						ImGui::PopID();
						
						// Sonraki slot pozisyonu
						if (col < slots_per_row - 1 && col < blocks_in_row - 1)
						{
							ImGui::SameLine(0, slot_spacing);
						}
					}
					
					// Yeni satır için cursor'ı ayarla
					if (row < total_rows - 1)
					{
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + slot_spacing);
						ImGui::SetCursorPosX(10.0f); // Window padding'e geri dön
					}
				}
				
				ImGui::End();
			}

			// Hotbar overlay - inventory penceresinin clip'ine takılmasın diye ayrı window
			{
				ImGuiWindowFlags hotbar_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground;
				ImGui::SetNextWindowPos(ImVec2(hotbar_x, hotbar_y), ImGuiCond_Always);
				ImGui::SetNextWindowSize(ImVec2(hotbar_width, 50.0f), ImGuiCond_Always);
				if (ImGui::Begin("##InventoryHotbarOverlay", nullptr, hotbar_flags))
				{
					RenderHotbar(hotbar_x, hotbar_y, hotbar_width);
					ImGui::End();
				}
			}
			
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor();
			
			// Drag & drop: Mouse'u takip eden blok texture'ı
			if (m_IsDragging && m_DraggedBlock != BlockType::Air)
			{
				ImVec2 mouse_pos = ImGui::GetMousePos();
				ImTextureID atlas_tex = (ImTextureID)(intptr_t)m_World->GetRenderer()->GetAtlasTexture()->GetTextureID();
				float atlas_w = (float)m_World->GetRenderer()->GetAtlasTexture()->GetWidth();
				float atlas_h = (float)m_World->GetRenderer()->GetAtlasTexture()->GetHeight();
				const std::array<uint16_t, 8>& uv_raw = BlockDatabase::GetBlockTexture(m_DraggedBlock, BlockFaceType::front);
				float x1 = uv_raw[2] / atlas_w;
				float y1 = uv_raw[3] / atlas_h;
				float x2 = uv_raw[0] / atlas_w;
				float y2 = uv_raw[5] / atlas_h;
				ImVec2 uv0 = ImVec2(x1, y2);
				ImVec2 uv1 = ImVec2(x2, y1);
				ImGui::GetForegroundDrawList()->AddImage(atlas_tex, ImVec2(mouse_pos.x - 20, mouse_pos.y - 20), ImVec2(mouse_pos.x + 20, mouse_pos.y + 20), uv0, uv1);
				
				// Envanter dışına tıklayınca drag'i iptal et
				if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
				{
					m_IsDragging = false;
					m_DraggedBlock = BlockType::Air;
				}
			}
		}
	}
	void Application::OnEvent(EventSystem::Event e)
	{
		switch (e.type)
		{
		case EventSystem::EventTypes::MousePress:
		{
			GUI::MouseButtonCallback(e.button, GLFW_PRESS, e.mods);
			break;
		}

		case EventSystem::EventTypes::MouseRelease:
		{
			GUI::MouseButtonCallback(e.button, GLFW_RELEASE, e.mods);
			break;
		}

		case EventSystem::EventTypes::WindowResize:
		{
			int wx = 0, wy = 0 ; 

			glfwGetFramebufferSize(m_Window, &wx, &wy);
			glViewport(0, 0, wx, wy);
			m_OrthagonalCamera.SetProjection(0.0f, wx, 0.0f, wy);
			e.wx = wx;
			e.wy = wy;

			break;
		}

		case EventSystem::EventTypes::KeyPress:
		{
			if (e.key == GLFW_KEY_ESCAPE)
			{
				if (m_GameState == GameState::PlayingState && m_InventoryOpen)
				{
					m_InventoryOpen = false;
					// Envanter kapandığında mouse pozisyonunu ekranın ortasına ayarla
					int w, h;
					glfwGetFramebufferSize(m_Window, &w, &h);
					double center_x = w / 2.0;
					double center_y = h / 2.0;
					glfwSetCursorPos(m_Window, center_x, center_y);
					
					// Kameranın mouse pozisyon takibini sıfırla
					if (m_World && m_World->p_Player)
					{
						m_World->p_Player->p_Camera.ResetMousePosition(center_x, center_y);
					}
					
					glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
				else
				{
					if (m_GameState == GameState::PlayingState) m_GameState = GameState::PauseState;
					else if (m_GameState == GameState::PauseState) m_GameState = GameState::PlayingState;
				}
			}
			else if (e.key == GLFW_KEY_F12 && e.mods & GLFW_MOD_CONTROL && e.mods & GLFW_MOD_SHIFT) m_ShowDebugInfo = !m_ShowDebugInfo;
			else if (e.key == GLFW_KEY_V)
			{
				m_VSync = !m_VSync;
				glfwSwapInterval(m_VSync);
			}
			else if (e.key == GLFW_KEY_TAB && m_GameState == GameState::PlayingState)
			{
				m_InventoryOpen = !m_InventoryOpen;
				if (m_InventoryOpen)
				{
					glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
				else
				{
					// Envanter kapandığında mouse pozisyonunu ekranın ortasına ayarla
					int w, h;
					glfwGetFramebufferSize(m_Window, &w, &h);
					double center_x = w / 2.0;
					double center_y = h / 2.0;
					glfwSetCursorPos(m_Window, center_x, center_y);
					
					// Kameranın mouse pozisyon takibini sıfırla
					if (m_World && m_World->p_Player)
					{
						m_World->p_Player->p_Camera.ResetMousePosition(center_x, center_y);
					}
					
					glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
			}
			// Hotbar slot seçimi (1-9 tuşları)
			else if (m_GameState == GameState::PlayingState && !m_InventoryOpen && e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_9)
			{
				int slot = e.key - GLFW_KEY_1; // Convert key to slot index (0-8)
				if (slot >= 0 && slot < 9)
				{
					m_CurrentHotbarSlot = slot;
					if (m_World && m_HotbarSlots[slot] != BlockType::Air)
					{
						m_World->p_Player->p_CurrentHeldBlock = (uint8_t)m_HotbarSlots[slot];
					}
					else if (m_World)
					{
						m_World->p_Player->p_CurrentHeldBlock = (uint8_t)BlockType::Air;
					}
				}
			}
			break;
			}
		
		case EventSystem::EventTypes::MouseScroll:
		{
			// Hotbar slot seçimi (mouse wheel)
			if (m_GameState == GameState::PlayingState && !m_InventoryOpen && m_World)
			{
				// Scroll up = next slot
				if (e.msy > 0.0f)
				{
					m_CurrentHotbarSlot = (m_CurrentHotbarSlot + 1) % 9;
				}
				// Scroll down = previous slot
				else if (e.msy < 0.0f)
				{
					m_CurrentHotbarSlot = (m_CurrentHotbarSlot - 1 + 9) % 9;
				}
				
				// Seçili slot'taki bloğu player'a ver
				if (m_HotbarSlots[m_CurrentHotbarSlot] != BlockType::Air)
				{
					m_World->p_Player->p_CurrentHeldBlock = (uint8_t)m_HotbarSlots[m_CurrentHotbarSlot];
				}
				else
				{
					m_World->p_Player->p_CurrentHeldBlock = (uint8_t)BlockType::Air;
				}
			}
			break;
		}
		}
		// Envanter açıkken mouse event'lerini işleme (kamera dönmesin)
		if (m_World && m_GameState == GameState::PlayingState) 
		{
			if (m_InventoryOpen)
			{
				// Envanter açıkken sadece window resize gibi önemli event'leri işle
				// Mouse move, mouse press, mouse scroll gibi event'leri işleme
				if (e.type == EventSystem::EventTypes::WindowResize)
				{
					m_World->OnEvent(e);
				}
				// Mouse event'lerini tamamen ignore et
			}
			else
			{
				// Normal oyun durumunda tüm event'leri işle
				m_World->OnEvent(e);
			}
		}
	}
	void Application::PollEvents()
	{
		for (int i = 0; i < m_EventQueue.size(); i++)
		{
			OnEvent(m_EventQueue[i]);
		}
		m_EventQueue.clear();
	}
	void Application::RenderHotbar(float x, float y, float width)
	{
		if (!m_World) return;
		
		ImTextureID atlas_tex = (ImTextureID)(intptr_t)m_World->GetRenderer()->GetAtlasTexture()->GetTextureID();
		float atlas_w = (float)m_World->GetRenderer()->GetAtlasTexture()->GetWidth();
		float atlas_h = (float)m_World->GetRenderer()->GetAtlasTexture()->GetHeight();
		
		float slot_size = 50.0f;
		float slot_spacing = 5.0f;
		
		// Screen space pozisyon kullan (hotbar'ı farklı UI pencerelerinden güvenle çizebilmek için)
		ImGui::SetCursorScreenPos(ImVec2(x, y));
		
		for (int i = 0; i < 9; i++)
		{
			BlockType block = m_HotbarSlots[i];
			bool is_selected = (m_CurrentHotbarSlot == i);
			
			ImGui::PushID(1000 + i); // Unique ID for hotbar slots
			ImVec2 p = ImGui::GetCursorScreenPos();
			
			// Slot arka planı
			ImGui::GetBackgroundDrawList()->AddRectFilled(p, ImVec2(p.x + slot_size, p.y + slot_size), IM_COL32(50, 50, 50, 200), 5.0f);
			if (is_selected) 
				ImGui::GetBackgroundDrawList()->AddRect(p, ImVec2(p.x + slot_size, p.y + slot_size), IM_COL32(255, 255, 255, 255), 5.0f, 0, 3.0f);
			
			// Drag & drop için invisible button (tüm slot için)
			ImGui::SetCursorScreenPos(p);
			ImGui::InvisibleButton("##HotbarSlot", ImVec2(slot_size, slot_size));
			
			// Blok render etme
			if (block != BlockType::Air)
			{
				const std::array<uint16_t, 8>& uv_raw = BlockDatabase::GetBlockTexture(block, BlockFaceType::front);
				float x1 = uv_raw[2] / atlas_w;
				float y1 = uv_raw[3] / atlas_h;
				float x2 = uv_raw[0] / atlas_w;
				float y2 = uv_raw[5] / atlas_h;
				ImVec2 uv0 = ImVec2(x1, y2);
				ImVec2 uv1 = ImVec2(x2, y1);
				
				// Blok texture'ı
				ImGui::GetBackgroundDrawList()->AddImage(atlas_tex, ImVec2(p.x + 5, p.y + 5), ImVec2(p.x + 45, p.y + 45), uv0, uv1);
				
				// Drag başlatma (hotbar'dan)
				if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !m_IsDragging)
				{
					m_IsDragging = true;
					m_DraggedBlock = block;
					m_HotbarSlots[i] = BlockType::Air;
				}
			}
			
			// Drop (hotbar'a - hem envanter'den hem hotbar'dan)
			ImVec2 mouse_pos = ImGui::GetMousePos();
			bool mouse_in_slot = (mouse_pos.x >= p.x && mouse_pos.x <= (p.x + slot_size) && mouse_pos.y >= p.y && mouse_pos.y <= (p.y + slot_size));
			if (m_IsDragging && mouse_in_slot && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				// Swap blocks
				BlockType temp = m_HotbarSlots[i];
				m_HotbarSlots[i] = m_DraggedBlock;
				m_DraggedBlock = temp;
				if (m_DraggedBlock == BlockType::Air)
					m_IsDragging = false;
			}
			
			// Seçim için tıklama
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !m_IsDragging)
			{
				m_CurrentHotbarSlot = i;
			}
			
			ImGui::PopID();
			
			// Sonraki slot pozisyonu
			if (i < 8)
			{
				ImGui::SameLine(0, slot_spacing);
			}
		}
		
		// Seçili slot'taki bloğu player'a ver
		if (m_HotbarSlots[m_CurrentHotbarSlot] != BlockType::Air)
		{
			m_World->p_Player->p_CurrentHeldBlock = (uint8_t)m_HotbarSlots[m_CurrentHotbarSlot];
		}
		else
		{
			m_World->p_Player->p_CurrentHeldBlock = (uint8_t)BlockType::Air;
		}
	}

	Application* GetOmniaApp()
	{
		return &OmniaApplication;
	}
}