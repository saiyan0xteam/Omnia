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
	constexpr float default_player_speed = 0.05f;
	constexpr float default_player_sensitivity = 0.25f;
	float ex_PlayerSpeed = default_player_speed;
	float ex_PlayerSensitivity = default_player_sensitivity;

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

		// Turn on V-Sync
		glfwSwapInterval(1);

		glewInit();

#ifndef NDEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(gl_debug_callback, nullptr);
#endif
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
		m_World->p_Player->p_Camera.SetPosition(glm::vec3(0, spawn_y, 0));

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
			bool show_cross = (m_GameState == GameState::PlayingState); 
			m_World->RenderWorld(show_cross);

			bool update_player_input = (m_GameState == GameState::PlayingState);
			
			// Always update world (generates chunks, lights, etc.)
			// but only process player input if playing
			if (m_GameState == GameState::PlayingState) 
				glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				
			m_World->OnUpdate(m_Window, update_player_input);

			if (m_GameState != GameState::PlayingState)
			{
				// Rotate camera slowly in menu / pause / other states
				float currentYaw = m_World->p_Player->p_Camera.GetYaw();
				m_World->p_Player->p_Camera.SetYaw(currentYaw + 10.0f * 0.016f);
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

		// For the world create menu
		static char input[64];
		static int seed = 0;
		static int world_type = 0;

		std::vector<std::string> Saves;

		if (m_GameState == GameState::PlayingState && m_World)
		{
			bool open = true;
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground;
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);

			//if (ImGui::Begin("Debug Text", &open, window_flags))
			//{
			//	stringstream ss;
			//	BlockType current_block = static_cast<BlockType>(m_World->p_Player->p_CurrentHeldBlock);
			//	const glm::vec3& pos = m_World->p_Player->p_Position;

			//	ss << "VSync : " << m_VSync << "\n";
			//	ss << "Current held block : " << BlockDatabase::GetBlockName(current_block).c_str() << "\n";
			//	ss << "Player Position =  X : " << pos.x << " | Y : " << pos.y << " | Z : " << pos.z << "\n"; 
			//	ss << "Player is colliding : " << m_World->p_Player->p_IsColliding << "\n";
			//	ss << "Freefly : " << m_World->p_Player->p_FreeFly << "\n";
			//	ss << "Chunk amount : " << m_World->m_ChunkCount << "\n";
			//	ss << "Chunks being rendered : " << m_World->p_ChunksRendered << "\n";
			//	ss << "Sun Position : " << m_World->GetSunPositionY() << "\n";  
			//	ImGui::Text(ss.str().c_str());
			//	
			//	if (m_ShowDebugInfo)
			//	{
			//		std::stringstream debug_ss;

			//		debug_ss << "This menu is experimental! It is still W.I.P as it is extremely platform specific\n";
			//		debug_ss << "Total CPU Used : " << m_ProcDebugInfo.cpu_usage << "\n";
			//		debug_ss << "Total Memory : " << m_ProcDebugInfo.total_mem << "  /  " << m_ProcDebugInfo.total_mem_used << "\n";
			//		debug_ss << "Total Virtual Memory : " << m_ProcDebugInfo.total_vm << "  /  " << m_ProcDebugInfo.total_vm_used << "\n";
			//		debug_ss << "CPU Usage : " << m_ProcDebugInfo.cpu_usage << "\n";
			//		ImGui::Text(debug_ss.str().c_str());
			//	}

			//	ImGui::End();
			//}




			if (ImGui::Begin("##Omnia-InGame", &open, window_flags)) {



				// -- Player Stats (Above Hotbar) --
				// Aligning bars to be above the hotbar
				float stats_width = 200.0f;
				float stats_start_x = 15.0f;
				float stats_start_y = h - 200.0f; 
				
				ImGui::SetCursorPos(ImVec2(stats_start_x, stats_start_y));
				
				// Helper to draw a styled progress bar
				auto DrawImGuiBar = [&](float val, float max_val, const char* label, ImVec4 color) {
					ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
					
					char overlay[32];
					sprintf(overlay, "%s %.0f/%.0f", label, val, max_val);
					
					ImGui::ProgressBar(val / max_val, ImVec2(stats_width, 18), overlay);
					
					ImGui::PopStyleColor(2);
					// Add a small spacing
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
					ImGui::SetCursorPosX(stats_start_x); 
				};

				DrawImGuiBar(m_World->p_Player->p_Health, 100.0f, "Health", ImVec4(0.9f, 0.3f, 0.23f, 1.0f)); // Red
				DrawImGuiBar(m_World->p_Player->p_Armor, 100.0f, "Armor", ImVec4(0.7f, 0.75f, 0.78f, 1.0f));  // Silver
				DrawImGuiBar(m_World->p_Player->p_Hunger, 100.0f, "Hunger", ImVec4(0.95f, 0.61f, 0.07f, 1.0f)); // Orange
				DrawImGuiBar(m_World->p_Player->p_Temperature, 100.0f, "Air", ImVec4(0.2f, 0.6f, 0.86f, 1.0f));   // Blue


				// -- Hotbar --
				static BlockType hotbar_blocks[] = {

					BlockType::Grass, BlockType::Dirt, BlockType::Stone, BlockType::Cobblestone,
					BlockType::OakLog, BlockType::OakPlanks, BlockType::Sand, BlockType::Cactus,
					BlockType::GlassWhite
				};

				float hotbar_width = 9 * 50.0f + 10 * 5.0f;
				float start_x = 15.0f;
				float start_y = h - 60.0f;

				ImGui::SetCursorPos(ImVec2(start_x, start_y));
				
				ImTextureID atlas_tex = (ImTextureID)(intptr_t)m_World->GetRenderer()->GetAtlasTexture()->GetTextureID();
				float atlas_w = (float)m_World->GetRenderer()->GetAtlasTexture()->GetWidth();
				float atlas_h = (float)m_World->GetRenderer()->GetAtlasTexture()->GetHeight();

				for (int i = 0; i < 9; i++) {
					BlockType block = hotbar_blocks[i];
					bool is_selected = (m_World->p_Player->p_CurrentHeldBlock == (uint8_t)block);
					
					ImGui::PushID(i);
					
					// Slot Background
					ImVec2 p = ImGui::GetCursorScreenPos();
					ImGui::GetBackgroundDrawList()->AddRectFilled(p, ImVec2(p.x + 50, p.y + 50), IM_COL32(50, 50, 50, 200), 5.0f);
					if (is_selected) {
						ImGui::GetBackgroundDrawList()->AddRect(p, ImVec2(p.x + 50, p.y + 50), IM_COL32(255, 255, 255, 255), 5.0f, 0, 3.0f);
					}



					// 2D Block Icon Rendering (Reverted from 3D due to aspect ratio issues)
					const std::array<uint16_t, 8>& uv_raw = BlockDatabase::GetBlockTexture(block, BlockFaceType::front);
					
					// Correcting UVs for vertical flip: y1 -> y2, y2 -> y1
					float x1 = uv_raw[2] / atlas_w;
					float y1 = uv_raw[3] / atlas_h;
					float x2 = uv_raw[0] / atlas_w;
					float y2 = uv_raw[5] / atlas_h;
					
					ImVec2 uv0 = ImVec2(x1, y2);
					ImVec2 uv1 = ImVec2(x2, y1);

					ImGui::SetCursorScreenPos(ImVec2(p.x + 5, p.y + 5));
					ImGui::Image(atlas_tex, ImVec2(40, 40), uv0, uv1);

					ImGui::PopID();
					ImGui::SetCursorScreenPos(ImVec2(p.x + 55, p.y));
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
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// m_Renderer2D->RenderQuad(glm::vec2(0, 0), &m_BlurMenuBackground, &m_OrthagonalCamera, w, h);
			bool open = true;
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;
			ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.7f);
			style.WindowBorderSize = 0.0f;
			style.WindowPadding = ImVec2(0, 0);
			if (ImGui::Begin("##Omnia-Settings", &open, window_flags))
			{
				ImGui::SliderFloat("Player Speed", &ex_PlayerSpeed, 0.01, 0.18f);
				ImGui::SliderFloat("Sensitivity", &ex_PlayerSensitivity, 0.01, 1.5f);
				ImGui::SliderInt("Render Distance", &renderdistance, 2, 16);

				ImGui::NewLine();
				ImGui::NewLine();

				if (ImGui::Button("Ok"))
				{
					m_GameState = prev_settings_state;
				}

				if (ImGui::Button("Reset"))
				{
					ex_PlayerSpeed = default_player_speed;
					ex_PlayerSensitivity = default_player_sensitivity;
				}

				ImGui::NewLine();

				ImGui::End();
			}

			m_World->SetRenderDistance(renderdistance);
		}

		else if (m_GameState == GameState::PauseState)
		{
			// Darken the background (World is already rendered in OnUpdate)
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2((float)w, (float)h), IM_COL32(0, 0, 0, 100)); // Dark overlay
			
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			bool open = true;

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;

			ImGui::SetNextWindowPos(ImVec2((w / 2) - 220 / 2, (h / 2) - 70 / 2), ImGuiCond_Always);

			if (ImGui::Begin("Pause Menu.", &open, window_flags))
			{

				if (ImGui::Button("RESUME", ImVec2(200, 48)))
				{
					m_GameState = GameState::PlayingState;
				}

				ImGui::NewLine();

				if (ImGui::Button("SETTINGS", ImVec2(200, 48)))
				{
					m_GameState = GameState::SettingsState;
				}

				ImGui::NewLine();

				if (ImGui::Button("QUIT", ImVec2(200, 48)))
				{
					WorldFileHandler::SaveWorld(m_World->GetName(), m_World);
					m_GameState = GameState::MenuState;

					delete m_World;
					// Recreate Menu World
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
								spawn_y = (float)y + 3.0f;
								break;
							}
						}
					}

					m_World->p_Player->p_Position = glm::vec3(0, spawn_y, 0); 
					m_World->p_Player->p_Camera.SetPosition(glm::vec3(0, spawn_y, 0));
				}

				ImGui::End();
			}
		}

		else if (m_GameState == GameState::WorldSelectState)
		{
			// m_Renderer2D->RenderQuad(glm::vec2(0, 0), &m_BlurMenuBackground, &m_OrthagonalCamera, w, h);

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

			ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(w - 20, h - 20), ImGuiCond_Always);

			if (ImGui::Begin("Menu", &open, window_flags))
			{

				if (ImGui::Button("Create new world.."))
				{
					m_GameState = GameState::WorldCreateState;
				}

				ImGui::SameLine();
				ImGui::Text("                     ");
				ImGui::SameLine();

				if (ImGui::Button("BACK"))
				{
					m_GameState = GameState::MenuState;
				}

				ImGui::Separator();
				ImGui::NewLine();

				for (int i = 0; i < Saves.size(); i++)
				{
					if (ImGui::Button(Saves.at(i).c_str()))
					{
						m_World = WorldFileHandler::LoadWorld(Saves.at(i));
						m_GameState = GameState::PlayingState;

						EventSystem::Event e;
						e.type = EventSystem::EventTypes::WindowResize;
						m_EventQueue.push_back(e);
						break;
					}

					ImGui::Separator();
				}

				ImGui::NewLine();

				ImGui::End();
			}
		}

		else if (m_GameState == GameState::WorldCreateState)
		{
// m_Renderer2D->RenderQuad(glm::vec2(0, 0), &m_BlurMenuBackground, &m_OrthagonalCamera, w, h);

			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			bool open = true;

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;

			ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(w - 20, h - 20), ImGuiCond_Always);

			if (ImGui::Begin("World Create", &open, window_flags))
			{

				if (ImGui::Button("BACK"))
				{
					m_GameState = GameState::WorldSelectState;
				}

				ImGui::Text("\n");
				ImGui::Separator();
				ImGui::Text("\n");
				ImGui::Text("Create your new world!");
				ImGui::Text("\n");
				ImGui::InputText("World Name", input, 63);
				ImGui::InputInt("Seed", &seed);
				ImGui::Text("\n\n");
				ImGui::RadioButton("Normal", &world_type, (int)WorldGenerationType::Generation_Normal);
				ImGui::RadioButton("Islands", &world_type, (int)WorldGenerationType::Generation_Islands);
				ImGui::RadioButton("Hilly", &world_type, (int)WorldGenerationType::Generation_Hilly);
				ImGui::RadioButton("Flat", &world_type, (int)WorldGenerationType::Generation_Flat);
				ImGui::RadioButton("Flat without Structures", &world_type, (int)WorldGenerationType::Generation_FlatWithoutStructures);

				ImGui::Text("\n\n");

				if (ImGui::Button("Create!", ImVec2(200,200)))
				{
					bool isValid = FilenameIsValid(input);

					if (isValid)
					{
						m_World = new World(seed, glm::vec2(w, h), input, static_cast<WorldGenerationType>(world_type));
						m_GameState = GameState::PlayingState;
						memset(input, '\0', 64);
					}

					else
					{
						Logger::LogToConsole("INVALID WORLD NAME GIVEN!");
					}
				}

				ImGui::End();
			}
		}

		else if (m_GameState == GameState::HelpState)
		{
// m_Renderer2D->RenderQuad(glm::vec2(0, 0), &m_BlurMenuBackground, &m_OrthagonalCamera, w, h);
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			bool open = true;

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;

			ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always);

			if (ImGui::Begin("Help Menu", 0, window_flags))
			{
				if (ImGui::Button("BACK"))
				{
					m_GameState = GameState::MenuState;
				}

				ImGui::Separator();
				ImGui::Text("\n");
				ImGui::Text("A Voxel-Based Sandbox Game");
				ImGui::Text("By Samuel Rasquinha (samuelrasquinha@gmail.com)");
				ImGui::Text("Discord : swr#1899");
				ImGui::Text("If you like this project. Please consider starring it on GitHub");
				ImGui::Text("All art and resources are not mine. Credits go to their respective owners");
				ImGui::Text("\n\n");
				ImGui::Text("Instructions : ");
				ImGui::Text("TOGGLE DEBUG MENU : F3");
				ImGui::Text("MOVEMENT - W S A D");
				ImGui::Text("FLY - SPACE/LEFT SHIFT");
				ImGui::Text("BLOCK EDITING - LEFT/RIGHT MOUSE BUTTONS");
				ImGui::Text("CHANGE CURRENT BLOCK - (Q) or (E)");
				ImGui::Text("TOGGLE VSYNC - (V)");
				ImGui::Text("TOGGLE COLLISION DETECTION / FREEFLY - (F)");
				ImGui::Text("RESET SUN - (G)");

				ImGui::End();
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
			if (e.key == GLFW_KEY_ESCAPE && m_GameState == GameState::PlayingState)
			{
				m_GameState = GameState::PauseState;
			}

			else if (e.key == GLFW_KEY_F3 && m_GameState == GameState::PlayingState)
			{
				m_ShowDebugInfo = !m_ShowDebugInfo;
			}

			else if (e.key == GLFW_KEY_ESCAPE && m_GameState == GameState::PauseState)
			{
				m_GameState = GameState::PlayingState;
			}

			else if (e.key == GLFW_KEY_V)
			{
				m_VSync = !m_VSync;
				glfwSwapInterval(m_VSync);
			}

			break;
		}
		}

		if (m_World && m_GameState == GameState::PlayingState)
		{
			m_World->OnEvent(e);
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

	Application* GetOmniaApp()
	{
		return &OmniaApplication;
	}
}
