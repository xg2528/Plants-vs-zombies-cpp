#include"pch.h"
#include"Spawn.h"
#include"Constants.h"
#include"AudioManager.h"
#include <cmath>
#include <algorithm>
extern AudioManager audio;
void Spawn::update(float delta) {
    // 最后一波音效分阶段播放
    if (lastWaveSoundStage == 1) {
        lastWaveSoundTimer -= delta;
        if (lastWaveSoundTimer <= 0.0f) {
            audio.playEffect("hugewave.mp3");
            lastWaveSoundStage = 2;
        }
    }

    timer += delta;
    if (timer >= frequency) {
        currentWave++;
        if (currentWave >= wave) {
            currentWave = wave;
            return;
        }
        // 最后一波：先播放finalwave
        if (currentWave == wave - 1 && lastWaveSoundStage == 0) {
            audio.playEffect("finalwave.mp3");
            lastWaveSoundStage = 1;
            lastWaveSoundTimer = 4.0f;  // 等finalwave播完再放hugewave
        }

        currentWeight = 0;
        const int maxZombiesPerWave = 50;
        int generatedCount = 0;
        int rowCounts[5] = { 0, 0, 0, 0, 0 };  // 每条路已分配的僵尸数

        while (currentWeight < spawnWeight[currentWave] && generatedCount < maxZombiesPerWave) {
            if (spawnType.empty()) break;

            // 计算最小cost
            int minCost = 9999;
            for (int t : spawnType) {
                int c = 0;
                switch (t) {
                case 0: c = 10; break;
                case 1: c = 20; break;
                case 3: c = 40; break;
                default: c = 10; break;
                }
                if (c < minCost) minCost = c;
            }
            if (currentWeight + minCost > spawnWeight[currentWave]) break;

            // 筛选能放入剩余权重的类型
            std::vector<int> validTypes;
            for (int t : spawnType) {
                int c = 0;
                switch (t) {
                case 0: c = 10; break;
                case 1: c = 20; break;
                case 3: c = 40; break;
                default: c = 10; break;
                }
                if (currentWeight + c <= spawnWeight[currentWave])
                    validTypes.push_back(t);
            }
            if (validTypes.empty()) break;

            // 加权随机：用 sqrt(cost) 缩小贵贱僵尸的权重差距
            int totalWeighted = 0;
            for (int t : validTypes) {
                int c = 0;
                switch (t) {
                case 0: c = 10; break; case 1: c = 20; break; case 3: c = 40; break;
                default: c = 10; break;
                }
                totalWeighted += (int)((spawnWeight[currentWave] - currentWeight) / std::sqrt((float)c));
            }
            int pick = rand() % totalWeighted;
            int type = validTypes[0];
            int accum = 0;
            for (int t : validTypes) {
                int c = 0;
                switch (t) {
                case 0: c = 10; break; case 1: c = 20; break; case 3: c = 40; break;
                default: c = 10; break;
                }
                accum += (int)((spawnWeight[currentWave] - currentWeight) / std::sqrt((float)c));
                if (pick < accum) { type = t; break; }
            }
            int cost = 0;
            switch (type) {
            case 0: cost = 10; break;
            case 1: cost = 20; break;
            case 3: cost = 40; break;
            default: cost = 10; break;
            }

            currentWeight += cost;

            // 加权随机选行：僵尸越少的行权重越高
            int maxCount = *std::max_element(rowCounts, rowCounts + 5);
            int totalRowWeight = 0;
            for (int i = 0; i < 5; i++) {
                totalRowWeight += (maxCount + 1 - rowCounts[i]);
            }
            int rowPick = rand() % totalRowWeight;
            int row = 0;
            int rowAccum = 0;
            for (int i = 0; i < 5; i++) {
                rowAccum += (maxCount + 1 - rowCounts[i]);
                if (rowPick < rowAccum) { row = i; break; }
            }
            rowCounts[row]++;

            float startX = GRID_WORLD_X + 8 * CELL_W + 100;
            float startY = GRID_WORLD_Y + row * CELL_H + (CELL_H - 144) / 2 - 25;

            switch (type) {
            case 0:
                spawnFinal.push_back(std::make_unique<NormalZombie>(row, startX, startY));
                break;
            case 1:
                spawnFinal.push_back(std::make_unique<ConeheadZombie>(row, startX, startY));
                break;
            case 3:
                startY = GRID_WORLD_Y + row * CELL_H + (CELL_H - 144) / 2 - 35;
                spawnFinal.push_back(std::make_unique<FootballZombie>(row, startX, startY));
                break;
            }
            generatedCount++;
        }

        timer = 0;
    }
}
