#include"pch.h"
#include"Spawn.h"
#include"Constants.h"
#include <cmath>
void Spawn::update(float delta) {
    timer += delta;
    if (timer >= frequency) {
        currentWave++;
        if (currentWave >= wave) {
            currentWave = wave;
            return;
        }

        currentWeight = 0;
        const int maxZombiesPerWave = 50;
        int generatedCount = 0;

        while (currentWeight < spawnWeight[currentWave] && generatedCount < maxZombiesPerWave) {
            if (spawnType.empty()) break;

            // Calculate min cost
            int minCost = 9999;
            for (int t : spawnType) {
                int c = 0;
                switch (t) {
                case 0: c = 10; break;
                case 1: c = 20; break;
                case 3: c = 50; break;
                default: c = 10; break;
                }
                if (c < minCost) minCost = c;
            }
            if (currentWeight + minCost > spawnWeight[currentWave]) break;

            // Filter valid types that fit within remaining weight
            std::vector<int> validTypes;
            for (int t : spawnType) {
                int c = 0;
                switch (t) {
                case 0: c = 10; break;
                case 1: c = 20; break;
                case 3: c = 50; break;
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
                case 0: c = 10; break; case 1: c = 20; break; case 3: c = 50; break;
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
                case 0: c = 10; break; case 1: c = 20; break; case 3: c = 50; break;
                default: c = 10; break;
                }
                accum += (int)((spawnWeight[currentWave] - currentWeight) / std::sqrt((float)c));
                if (pick < accum) { type = t; break; }
            }
            int cost = 0;
            switch (type) {
            case 0: cost = 10; break;
            case 1: cost = 20; break;
            case 3: cost = 50; break;
            default: cost = 10; break;
            }

            currentWeight += cost;

            int row = rand() % 5;
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
