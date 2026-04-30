#pragma once
#include "../shared.h"
#include "CTexture.h"

class CPlayer {
public:
  enum Direction { DOWN, UP, LEFT, RIGHT };

  Direction dir = DOWN;
  bool isMoving = false;
  float speed = 200.0f;
  Vector2 pos;
  CTexture idle_down;
  CTexture idle_up;
  CTexture idle_left;
  CTexture idle_right;
  CTexture walk_down;
  CTexture walk_up;
  CTexture walk_left;
  CTexture walk_right;
  CTexture *currentAnim = nullptr;

  void Power() {
    Logger::Success("Interfaces", "IPlayer Successfully Powered!");
    idle_down.Load(
        (WorkingDirectory + "\\textures\\character\\adelina_idle_down1.png")
            .c_str(),
        (WorkingDirectory + "\\textures\\character\\adelina_idle_down2.png")
            .c_str());
    idle_up.Load(
        (WorkingDirectory + "\\textures\\character\\adelina_idle_up1.png")
            .c_str(),
        (WorkingDirectory + "\\textures\\character\\adelina_idle_up2.png")
            .c_str());
    idle_left.Load(
        (WorkingDirectory + "\\textures\\character\\adelina_idle_left1.png")
            .c_str(),
        (WorkingDirectory + "\\textures\\character\\adelina_idle_left2.png")
            .c_str());
    idle_right.Load(
        (WorkingDirectory + "\\textures\\character\\adelina_idle_right1.png")
            .c_str(),
        (WorkingDirectory + "\\textures\\character\\adelina_idle_right2.png")
            .c_str());
    walk_down.Load(
        (WorkingDirectory + "\\textures\\character\\adelina_walk_down1.png")
            .c_str(),
        (WorkingDirectory + "\\textures\\character\\adelina_walk_down2.png")
            .c_str());
    walk_up.Load(
        (WorkingDirectory + "\\textures\\character\\adelina_walk_up1.png")
            .c_str(),
        (WorkingDirectory + "\\textures\\character\\adelina_walk_up2.png")
            .c_str());
    walk_left.Load(
        (WorkingDirectory + "\\textures\\character\\adelina_walk_left1.png")
            .c_str(),
        (WorkingDirectory + "\\textures\\character\\adelina_walk_left2.png")
            .c_str());
    walk_right.Load(
        (WorkingDirectory + "\\textures\\character\\adelina_walk_right1.png")
            .c_str(),
        (WorkingDirectory + "\\textures\\character\\adelina_walk_right2.png")
            .c_str());
    pos = {0, 0};
  }

  void Fire(float dt) {
    Vector2 move = {0, 0};
    if (IsKeyDown(KEY_D))
      move.x += 1;
    if (IsKeyDown(KEY_A))
      move.x -= 1;
    if (IsKeyDown(KEY_W))
      move.y -= 1;
    if (IsKeyDown(KEY_S))
      move.y += 1;
    isMoving = (move.x != 0 || move.y != 0);
    if (IsKeyDown(KEY_D))
      dir = RIGHT;
    if (IsKeyDown(KEY_A))
      dir = LEFT;
    if (IsKeyDown(KEY_W))
      dir = UP;
    if (IsKeyDown(KEY_S))
      dir = DOWN;
    if (isMoving) {
      move = Vector2Normalize(move);
      pos.x += move.x * speed * dt;
      pos.y += move.y * speed * dt;
    }
    if (isMoving) {
      switch (dir) {
      case DOWN:
        currentAnim = &walk_down;
        break;
      case UP:
        currentAnim = &walk_up;
        break;
      case LEFT:
        currentAnim = &walk_left;
        break;
      case RIGHT:
        currentAnim = &walk_right;
        break;
      }
    } else {
      switch (dir) {
      case DOWN:
        currentAnim = &idle_down;
        break;
      case UP:
        currentAnim = &idle_up;
        break;
      case LEFT:
        currentAnim = &idle_left;
        break;
      case RIGHT:
        currentAnim = &idle_right;
        break;
      }
    }
    currentAnim->AnimSwap(dt);
  }

  void Paint() {
    DrawTexture(currentAnim->GetTexture(), (int)pos.x, (int)pos.y, WHITE);
  }

  void Destroy() {
    UnloadTexture(idle_down.tex[0]);
    UnloadTexture(idle_down.tex[1]);
    UnloadTexture(idle_up.tex[0]);
    UnloadTexture(idle_up.tex[1]);
    UnloadTexture(idle_left.tex[0]);
    UnloadTexture(idle_left.tex[1]);
    UnloadTexture(idle_right.tex[0]);
    UnloadTexture(idle_right.tex[1]);
    UnloadTexture(walk_down.tex[0]);
    UnloadTexture(walk_down.tex[1]);
    UnloadTexture(walk_up.tex[0]);
    UnloadTexture(walk_up.tex[1]);
    UnloadTexture(walk_left.tex[0]);
    UnloadTexture(walk_left.tex[1]);
    UnloadTexture(walk_right.tex[0]);
    UnloadTexture(walk_right.tex[1]);
    Logger::Success("Interfaces", "IPlayer Successfully Destroyed!");
  }
};