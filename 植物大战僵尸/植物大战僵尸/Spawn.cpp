#include"pch.h"
#include"Spawn.h"
#include"Constants.h"
void Spawn::update(float delta) {
    // 这里可以实现每隔 frequency 秒生成一波僵尸的逻辑
    // 根据 spawnType 和 spawnWeight 来决定生成哪些类型的僵尸
    // 例如，可以使用 spawnWeight 来随机选择生成哪一波的僵尸
    timer += delta;
    if (timer >= frequency) {
        // 生成一波僵尸的逻辑
        currentWave++;                     // 进入下一波（第一波时从 -1 变为 0）
        if (currentWave >= wave) {         // 所有波次已生成完毕
            currentWave = wave;            // 防止越界，不再生成
            return;
        }

        // 重置本波累计分数
        currentWeight = 0;

        // 每波最多生成 50 个僵尸，防止无限循环或过多僵尸
        const int maxZombiesPerWave = 50;
        int generatedCount = 0;

        // 循环生成僵尸，直到累计分数达到本波总分阈值
        while (currentWeight < spawnWeight[currentWave] && generatedCount < maxZombiesPerWave) {
            // 确保 spawnType 非空
            if (spawnType.empty()) {
                break;
            }
            // 随机选择一种僵尸类型
            int idx = rand() % spawnType.size();
            int type = spawnType[idx];
            int cost = 0;

            // 根据类型确定分数（此处硬编码，可根据需要修改）
            switch (type) {
            case 0: cost = 10; break;      // 普通僵尸
	            case 1: cost = 20; break;      // ConeheadZombie
                // 其他类型...
            default: cost = 10; break;
            }

            currentWeight += cost;

            // 随机行（0-4）
            int row = rand() % 5;
            // 计算起始世界坐标：最右侧格子右侧偏移100像素，Y方向垂直居中
            float startX = GRID_WORLD_X + 8 * CELL_W + 100;
            float startY = GRID_WORLD_Y + row * CELL_H + (CELL_H - 144) / 2-25; // 减25像素修正

            // 创建僵尸对象
            switch (type) {
            case 0: // 普通僵尸
            {
                spawnFinal.push_back(std::make_unique<NormalZombie>(row, startX, startY));
                break;
            }
            case 1: // 旗帜僵尸或其他类型
	                spawnFinal.push_back(std::make_unique<ConeheadZombie>(row, startX, startY));
                break;
                // 其他类型...
            }
            generatedCount++;
        }

        timer = 0; // 重置计时器
    }

    if (currentWave >= wave) {
        // 所有波次的僵尸都已生成，可以在这里处理游戏胜利或其他逻辑
    }
}




//直接在playing的render使用，这一段废弃了void Spawn::render() {
	// 这里可以实现生成的僵尸的绘制逻辑
//}