#pragma once

#include <iostream>
#include <string>

#include "../FpsCamera.h"
#include "../Block.h"
#include "../Application/Events.h"

namespace Omnia
{
	class World;
	
	Block* GetWorldBlock(const glm::vec3& block_pos);  

	class Player
	{
	public : 
		static constexpr float EyeOffsetY = 0.72f;

		Player(float wx, float wy) : p_Camera(70.0f, wx / wy, 0.1, 500)
		{

		}

		void OnUpdate(GLFWwindow* window, float deltaTime);
		void OnEvent(EventSystem::Event e);
		bool TestBlockCollision(const glm::vec3& position);

		FPSCamera p_Camera;
		glm::vec3 p_Position;
		World* p_World;
		std::uint8_t p_CurrentHeldBlock = 0;
		bool p_IsColliding = false;
		bool p_FreeFly = false;
		bool p_CreativeFly = false;

		float p_Health = 100.0f;
		float p_Armor = 100.0f;
		float p_Hunger = 100.0f;
		float p_Temperature = 100.0f;

	private : 

		bool m_IsJumping = false;
		bool m_IsFalling = false;
		double m_LastShiftSpacePressTime = 0.0;
		bool m_WasSpaceDown = false;
		float m_VerticalVelocity = 0.0f;
		bool m_IsGrounded = false;
	};
}