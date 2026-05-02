#pragma once
#include<easyx.h>
#include"GameState.h"
#include "AudioManager.h"
#include"Game.h"
#include"Playing.h"

class Menu :public GameState
{
public:
	Menu();                           // 构造函数可以简单置空
	void onEnter() override;          // 加载资源、播放音乐
	void onExit() override;           // 释放资源、停止音乐
	void handleInput() override;      // 处理输入（你的原有代码）
	void update() override;           // 可以空实现
	void render() override;           // 绘制（你的原有代码）
	IMAGE& getSurface() {return surface; }

private:
	IMAGE surface;                // 背景图片（墓碑主体）
	struct Btn
	{
		IMAGE img;// 按钮图片（包含上下两状态）
		int btnX=0, btnY=0;               // 按钮绘制位置
		bool isHover=false;                 // 鼠标是否悬停在按钮上
		bool lastHover=false;               // 上一次的悬停状态
		bool isPressed = false;               // 鼠标是否按下
		int id = 0;
	};
	std::vector<Btn> buttons;
	Btn btnStart;
	Btn btnAlmanac;
	
};