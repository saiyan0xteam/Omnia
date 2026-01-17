#include "Player.h"

namespace Omnia
{
	/*
	The below externs are defined in application.cpp and are the variables controled via the setting menu.
	I know using externs is evil but I was lazy leave me alone :C
	*/

	extern float ex_PlayerSpeed;
	extern float ex_PlayerSensitivity;

	void Player::OnUpdate(GLFWwindow* window, float deltaTime)
	{
		// Scale camera speed by delta time for frame-independent movement
		const float camera_speed = ex_PlayerSpeed * deltaTime * 60.0f; // Multiply by 60 to maintain similar feel at 60 FPS

		p_Camera.ResetAcceleration();
		FPSCamera cam = p_Camera;

		if (p_Camera.GetSensitivity() != ex_PlayerSensitivity)
		{
			p_Camera.SetSensitivity(ex_PlayerSensitivity);
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			// Take the cross product of the camera's right and up.
			glm::vec3 front = -glm::cross(p_Camera.GetRight(), p_Camera.GetUp());
			p_Camera.ApplyAcceleration(front * camera_speed);
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			glm::vec3 back = glm::cross(p_Camera.GetRight(), p_Camera.GetUp());
			p_Camera.ApplyAcceleration(back * camera_speed);
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			p_Camera.ApplyAcceleration(-(p_Camera.GetRight() * camera_speed));
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			p_Camera.ApplyAcceleration(p_Camera.GetRight() * camera_speed);
		}

		// Creative fly toggle: Shift + double Space
		const bool is_shift_down = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
		const bool is_space_down = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
		const bool space_pressed = (is_space_down && !m_WasSpaceDown);
		m_WasSpaceDown = is_space_down;
		if (is_shift_down && space_pressed)
		{
			const double now = glfwGetTime();
			const double double_tap_threshold_s = 0.30;
			if (m_LastShiftSpacePressTime > 0.0 && (now - m_LastShiftSpacePressTime) <= double_tap_threshold_s)
			{
				p_CreativeFly = !p_CreativeFly;
				m_VerticalVelocity = 0.0f;
				m_LastShiftSpacePressTime = 0.0;
			}
			else
			{
				m_LastShiftSpacePressTime = now;
			}
		}

		const bool allow_collision_physics = (!p_FreeFly && !p_CreativeFly);

		// Vertical movement when creative fly OR noclip is enabled
		if (p_CreativeFly || p_FreeFly)
		{
			if (is_space_down)
			{
				p_Camera.ApplyAcceleration(p_Camera.GetUp() * camera_speed);
			}

			if (is_shift_down)
			{
				p_Camera.ApplyAcceleration(-(p_Camera.GetUp() * camera_speed));
			}
		}
		else if (allow_collision_physics)
		{
			// Jump only when grounded
			if (space_pressed && m_IsGrounded)
			{
				m_IsGrounded = false;
				m_VerticalVelocity = 6.0f;
			}
		}

		p_Camera.OnUpdate();

		// Apply gravity when neither noclip nor creative flight is enabled
		if (allow_collision_physics)
		{
			const float gravity = -18.0f;
			m_VerticalVelocity += gravity * deltaTime;
			glm::vec3* camera_pos = (glm::vec3*)&p_Camera.GetPosition();
			camera_pos->y += m_VerticalVelocity * deltaTime;
		}
		else
		{
			m_IsGrounded = false;
			m_VerticalVelocity = 0.0f;
		}

		glm::vec3 new_eye_pos = p_Camera.GetPosition();
		glm::vec3 old_eye_pos = cam.GetPosition();
		glm::vec3 new_center_pos = new_eye_pos - glm::vec3(0.0f, EyeOffsetY, 0.0f);
		glm::vec3 old_center_pos = old_eye_pos - glm::vec3(0.0f, EyeOffsetY, 0.0f);

		if (new_eye_pos != old_eye_pos)
		{
			glm::vec3* camera_pos = (glm::vec3*)&p_Camera.GetPosition();

			if (TestBlockCollision(glm::vec3(new_center_pos.x, old_center_pos.y, old_center_pos.z)))
			{
				camera_pos->x = old_eye_pos.x;
				p_Camera.ResetVelocity();
				p_Camera.ResetAcceleration();
			}

			if (TestBlockCollision(glm::vec3(old_center_pos.x, old_center_pos.y, new_center_pos.z)))
			{
				camera_pos->z = old_eye_pos.z;
				p_Camera.ResetVelocity();
				p_Camera.ResetAcceleration();
			}

			if (TestBlockCollision(glm::vec3(old_center_pos.x, new_center_pos.y, old_center_pos.z)))
			{
				camera_pos->y = old_eye_pos.y;
				if (allow_collision_physics)
				{
					// Ground / ceiling hit
					m_IsGrounded = (new_center_pos.y < old_center_pos.y);
					m_VerticalVelocity = 0.0f;
				}
				p_Camera.ResetVelocity();
				p_Camera.ResetAcceleration();
			}
			else
			{
				if (allow_collision_physics)
				{
					m_IsGrounded = false;
				}
			}

			p_Camera.Refresh();
		}

		// Update the player's position
		p_Position = p_Camera.GetPosition() - glm::vec3(0.0f, EyeOffsetY, 0.0f);
	}


	void Player::OnEvent(EventSystem::Event e)
	{
		// Hotbar kontrolü artık Application'da yapılıyor
		
		if (e.type == EventSystem::EventTypes::KeyPress)
		{
			if (e.key == GLFW_KEY_F)
			{
				p_FreeFly = !p_FreeFly;
				m_VerticalVelocity = 0.0f;
			}
		}

		else if (e.type == EventSystem::EventTypes::MouseMove)
		{
			p_Camera.UpdateOnMouseMovement(e.mx, e.my);
		}
	}

	static bool Test3DAABBCollision(const glm::vec3& pos_1, const glm::vec3& dim_1, const glm::vec3& pos_2, const glm::vec3& dim_2)
	{
		if (pos_1.x < pos_2.x + dim_2.x &&
			pos_1.x + dim_1.x > pos_2.x &&
			pos_1.y < pos_2.y + dim_2.y &&
			pos_1.y + dim_1.y > pos_2.y &&
			pos_1.z < pos_2.z + dim_2.z &&
			pos_1.z + dim_1.z > pos_2.z)
		{
			return true;
		}

		return false;
	}

	bool Player::TestBlockCollision(const glm::vec3& position)
	{
		if (p_FreeFly)
		{
			return false;
		}

		const glm::vec3 player_dim = glm::vec3(0.9f, 1.8f, 0.9f);
		const glm::vec3 player_half = player_dim * 0.5f;
		glm::vec3 pos = glm::vec3(
			position.x - player_half.x,
			position.y - player_half.y,
			position.z - player_half.z);

		glm::ivec3 player_block = {
			(int)floor(pos.x),
			(int)floor(pos.y),
			(int)floor(pos.z)
		};

		const glm::ivec3 block_range = { 2, 2, 2 };

		for (int i = player_block.x - block_range.x; i < player_block.x + block_range.x; i++)
			for (int j = player_block.y - block_range.y; j < player_block.y + block_range.y; j++)
				for (int k = player_block.z - block_range.z; k < player_block.z + block_range.z; k++)
				{
					if (j < CHUNK_SIZE_Y && j >= 0)
					{
						Block* block = GetWorldBlock(glm::vec3(i, j, k));

						if (block && block->Collidable())
						{
							if (Test3DAABBCollision(pos, player_dim, glm::vec3(i, j, k), glm::vec3(1, 1, 1)))
							{
								return true;
							}
						}
					}
				}

		return false;

	}
}