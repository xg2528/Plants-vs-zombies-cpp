#pragma once
#include <easyx.h>

class LevelMeter {
public:
	LevelMeter();
	void loadImages();
	void update(int currentWave, int totalWaves);
	void draw() const;

private:
	IMAGE emptyBar;       // empty progress bar background
	IMAGE fullBar;        // filled progress bar
	IMAGE levelText;      // "Level Progress" text
	IMAGE zombieHead;     // zombie head on the bar
	IMAGE flag;           // flag at the end of the bar

	float progress;       // 0.0 to 1.0
	int barX, barY;       // position of the bar
};
