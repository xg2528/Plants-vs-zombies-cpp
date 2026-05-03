#include "pch.h"
#include "LevelMeter.h"

extern void putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH);

LevelMeter::LevelMeter() : progress(0.0f), barX(0), barY(0) {
}

void LevelMeter::loadImages() {
	loadimage(&emptyBar, _T("resource/images/interface/FlagMeterEmpty.png"));
	loadimage(&fullBar, _T("resource/images/interface/FlagMeterFull.png"));
	loadimage(&levelText, _T("resource/images/interface/FlagMeterLevelProgress.png"));
	loadimage(&zombieHead, _T("resource/images/interface/FlagMeterParts1.png"));
	loadimage(&flag, _T("resource/images/interface/FlagMeterParts2.png"));

	// 位置：画面右下角
	barX = 900 - emptyBar.getwidth() - 20;
	barY = 565;
}

void LevelMeter::update(int currentWave, int totalWaves) {
	if (totalWaves <= 0) {
		progress = 0;
		return;
	}
	if (currentWave < 0) {
		progress = 0;
	} else if (currentWave >= totalWaves) {
		progress = 1.0f;
	} else {
		progress = (float)(currentWave + 1) / totalWaves;
	}
}

void LevelMeter::draw() const {
	// 绘制空进度条背景
	putimagePng(barX, barY, &emptyBar, 0, 0, emptyBar.getwidth(), emptyBar.getheight());

	// 绘制已填充部分（从右往左填充）
	int fillWidth = (int)(emptyBar.getwidth() * progress);
	if (fillWidth > 0) {
		// 填充从右侧开始
		int fillX = barX + emptyBar.getwidth() - fillWidth;
		putimagePng(fillX, barY, &fullBar,
		            emptyBar.getwidth() - fillWidth, 0, fillWidth, fullBar.getheight());
	}

	// "关卡进度" 文字底部与进度条底边重合（先绘制，在旗帜和僵尸头下面）
	int textX = barX + (emptyBar.getwidth() - levelText.getwidth()) / 2;
	int textY = barY + emptyBar.getheight() - levelText.getheight();
	putimagePng(textX, textY, &levelText, 0, 0, levelText.getwidth(), levelText.getheight());

	// 旗帜在进度条左端（在文字上面）
	int flagX = barX;
	int flagY = barY + (emptyBar.getheight() - flag.getheight()) / 2;
	putimagePng(flagX, flagY, &flag, 0, 0, flag.getwidth(), flag.getheight());

	// 僵尸头从右往左移动（在最上层）
	int headX = barX + emptyBar.getwidth() - zombieHead.getwidth()
	            - (int)((emptyBar.getwidth() - zombieHead.getwidth()) * progress);
	int headY = barY + (emptyBar.getheight() - zombieHead.getheight()) / 2;
	putimagePng(headX, headY, &zombieHead, 0, 0, zombieHead.getwidth(), zombieHead.getheight());
}
