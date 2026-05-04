#pragma once
#include "GameState.h"
#include <easyx.h>
#include<memory>
#include <chrono>
#include "Spawn.h"
#include"LevelData.h"
#include"LevelMeter.h"
#include "Bullet.h"

class PlayingState : public GameState {
public:
	void onEnter() override;
	void onExit() override;
	void handleInput() override;
	void update() override;
	void render() override;

private:
	IMAGE background;
	IMAGE sunBack;
	float cameraX=0.0f;
	int scrollState=0;
	int currentMusic=0;
	struct Card {
		IMAGE img;
		IMAGE plantImg;
		int x=0, y=0;
		int cost=0;
		bool isSelected=false;
		bool isCooling=false;
		float coolingRemain=0.0f;
		float totalCoolTime=7500.0f;
		int plantType=0;
	};
	std::vector<Card> cards;
	int currentSun=150;
	Spawn spawn;
	LevelData levelData;
	std::vector<std::unique_ptr<Bullet>> bullets;
	struct PreviewZombie {
	public:
		int imgIndex;
		float worldX;
		float worldY;
	};
	std::vector<IMAGE> previewImages;
	std::vector<PreviewZombie> previewZombies;

	IMAGE shovelImg;
	IMAGE shovelBackImg;
	bool shovelSelected = false;
	int shovelX = 500;
	int shovelY = 10;
	LevelMeter levelMeter;

	// 帧计时（避免 static 局部变量重入问题）
	int frameCounter = 0;
	std::chrono::steady_clock::time_point lastTime;
	std::chrono::steady_clock::time_point plantStartTime;
	float sunSpawnTimer = 0.0f;
	bool gameLost = false;
};