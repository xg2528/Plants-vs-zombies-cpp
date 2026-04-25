// Constants.cpp
#include"pch.h"
#include "Constants.h"
// 网格的左上角世界坐标（相对于背景图片）
const int GRID_WORLD_X = 252;
const int GRID_WORLD_Y = 82;
// 网格尺寸（根据右下角计算）
const int GRID_WIDTH = 999 - 252;   // 747
const int GRID_HEIGHT = 571 - 82;   // 489
// 9列5行
const int GRID_COLS = 9;
const int GRID_ROWS = 5;
// 每个格子的宽高（浮点数，用于精确计算）
const float CELL_W = (float)GRID_WIDTH / GRID_COLS;   // 约 83.0
const float CELL_H = (float)GRID_HEIGHT / GRID_ROWS;   // 约 97.8