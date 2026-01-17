#pragma once
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLEW_STATIC
#define IMGUI_DEFINE_MATH_OPERATORS
#define NOMINMAX
#include <Windows.h>
#include <GL/glew.h>
#include <glfw3.h>
#include <glfw3native.h>
#include <cstdio>
#include "../Utils/fontindex.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include "../OpenGL Classes/Fps.h"
#include "Events.h"
#include "../File Handling/WorldFileHandler.h"
#include "../World/Skybox.h"
#include "../Block.h"
#include "../Chunk.h"
#include "../UI/GUI.h"
#include "../Clouds/Cloud.h"
#include "PlatformSpecific.h"

namespace Omnia
{
	enum GameState : uint8_t
	{
		MenuState = 0,
		WorldSelectState,
		WorldCreateState,
		PlayingState,
		PauseState,
		HelpState,
		SettingsState,
		CommandMenuState
	};
	class Application
	{
	public : 
		GLFWwindow* m_Window;
		Application();
		~Application();
		void OnUpdate();
		inline World* GetWorld() { return m_World; }
		GameState GetState() { return m_GameState; }
	private :
		void OnImGuiRender();
		void OnEvent(EventSystem::Event e);
		void PollEvents();
		void RenderHotbar(float x, float y, float width);
		World* m_World;
		std::vector<EventSystem::Event> m_EventQueue;
		bool m_CursorLocked;
		bool m_VSync = true;
		bool m_ShowDebugInfo = false;
		bool m_InventoryOpen = false;
		GameState m_GameState;
		// Hotbar system
		BlockType m_HotbarSlots[9] = { BlockType::Air, BlockType::Air, BlockType::Air, BlockType::Air, BlockType::Air, BlockType::Air, BlockType::Air, BlockType::Air, BlockType::Air };
		int m_CurrentHotbarSlot = 0;
		// Drag & drop system
		bool m_IsDragging = false;
		BlockType m_DraggedBlock = BlockType::Air;
		Texture m_BlurMenuBackground;
		Renderer2D* m_Renderer2D;
		OrthographicCamera m_OrthagonalCamera;
		PlatformSpecific::ProcessDebugInfo m_ProcDebugInfo;
		int logoWidth, logoHeight;
		ImTextureID omnialogo;
		std::string menuTitle = "OMNIA";
		std::string menuDesc = "A Voxel-Based Sandbox Game Created by Saiyan0x";
		float m_DeltaTime = 0.0f;
		float m_LastFrameTime = 0.0f;
	};
	extern Application OmniaApplication;
	Application* GetOmniaApp();
}