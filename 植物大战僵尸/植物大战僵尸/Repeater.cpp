#include "pch.h"
#include "Plants.h"
#include "Spawn.h"
#include "Constants.h"
#include "Bullet.h"

extern void putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH);

void Repeater::loadImgs() {
	Img.clear();
	for (int i = 1; i <= m_pngNum; ++i) {
		IMAGE image;
		std::wstring wpath;
		wpath = L"resource/images/Plants/Repeater/frame_";
		if (i < 10)
			wpath += L"000" + std::to_wstring(i);
		else if (i < 100)
			wpath += L"00" + std::to_wstring(i);
		else
			wpath += L"0" + std::to_wstring(i);
		wpath += L".png";
		if (loadimage(&image, wpath.c_str()) == 0) {
			printf("Repeater frame loaded\n");
		} else {
			printf("Failed to load: %S\n", wpath.c_str());
		}
		Img.push_back(image);
	}
}

Repeater::Repeater(int r, int c) : Plants(15) {
	row = r;
	col = c;
	type = 2;
	shootTimer = 0.0f;
	shootInterval = 1.8f;
	secondPeaTimer = -1.0f;
	fireSecondPea = false;
	animInterval = 0.1f;
	currentFrame = 0;
	animTimer = 0.0f;
	loadImgs();
}

void Repeater::update(float delta) {
	animTimer += delta;
	if (animTimer >= animInterval) {
		animTimer -= animInterval;
		currentFrame = (currentFrame + 1) % m_pngNum;
	}
	if (secondPeaTimer > 0) {
		secondPeaTimer -= delta;
		if (secondPeaTimer <= 0) {
			fireSecondPea = true;
			secondPeaTimer = -1.0f;
		}
	}
}

void Repeater::draw(int x, int y) {
	if (!Img.empty()) {
		putimagePng(x, y, &Img[currentFrame], 0, 0, Img[currentFrame].getwidth(), Img[currentFrame].getheight());
	}
}

void Repeater::attack(float delta, const std::vector<std::unique_ptr<Zombies>>& zombies) {
	float plantWorldX = GRID_WORLD_X + col * CELL_W;
	bool isAttacking = false;
	for (auto& zombie : zombies) {
		if (zombie->row == row && !zombie->isDead() && zombie->worldX > plantWorldX) {
			isAttacking = true;
			break;
		}
	}
	if (isAttacking) {
		if (shootTimer > 0) shootTimer -= delta;
		else {
			extern std::vector<std::unique_ptr<Bullet>> g_bullets;
			float bulletX = GRID_WORLD_X + col * CELL_W + 40;
			float bulletY = GRID_WORLD_Y + row * CELL_H + 20;
			// 发射第一颗豌豆
			g_bullets.push_back(std::make_unique<PeaBullet>(bulletX, bulletY, row, 30));
			printf("Repeater at (%d,%d) shoots first pea!\n", row, col);
			secondPeaTimer = 0.2f;
			shootTimer = shootInterval;
		}
	}
	// 发射第二颗豌豆
	if (fireSecondPea) {
		extern std::vector<std::unique_ptr<Bullet>> g_bullets;
		float bulletX = GRID_WORLD_X + col * CELL_W + 40;
		float bulletY = GRID_WORLD_Y + row * CELL_H + 25;
		g_bullets.push_back(std::make_unique<PeaBullet>(bulletX, bulletY, row, 30));
		printf("Repeater at (%d,%d) shoots second pea!\n", row, col);
		fireSecondPea = false;
	}
}
