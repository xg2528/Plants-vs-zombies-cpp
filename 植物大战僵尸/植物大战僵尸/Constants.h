// Constants.h
#pragma once
constexpr int GRID_ROWS = 5;
constexpr int GRID_COLS = 9;
constexpr int GRID_WORLD_X = 252;
constexpr int GRID_WORLD_Y = 82;
constexpr int GRID_WIDTH = 999 - 252;   // 747
constexpr int GRID_HEIGHT = 571 - 82;   // 489
constexpr float CELL_W = (float)GRID_WIDTH / GRID_COLS;   // Լ 83.0
constexpr float CELL_H = (float)GRID_HEIGHT / GRID_ROWS;   // Լ 97.8
const int ZOMBIE_COLLIDE_OFFSET = 130;  // 僵尸嘴部位置，贴近植物时才触发啃咬
