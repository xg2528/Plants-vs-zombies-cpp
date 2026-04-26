#include "pch.h"
#include "LevelData.h"
#include <fstream>
#include <sstream>
#include <iostream>

LevelData::LevelData() {
    loadFromFile();
}

void LevelData::loadFromFile() {
    std::ifstream file("leveldata.txt");
    if (!file.is_open()) {
        createDefaultFile();
        file.open("leveldata.txt");
        if (!file.is_open()) {
            std::cerr << "Failed to open leveldata.txt" << std::endl;
            return;
        }
    }

    std::string line;
    // 读取第一行：波数
    if (std::getline(file, line)) {
        waveCount = std::stoi(line);
    }
    else {
        createDefaultFile();
        file.close();
        file.open("leveldata.txt");
        std::getline(file, line);
        waveCount = std::stoi(line);
    }

    // 读取第二行：僵尸类型（逗号分隔）
    if (std::getline(file, line)) {
        types.clear();
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ',')) {
            types.push_back(std::stoi(token));
        }
    }

    // 读取第三行：每波权重（逗号分隔）
    if (std::getline(file, line)) {
        weights.clear();
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ',')) {
            weights.push_back(std::stoi(token));
        }
    }

    file.close();
}

void LevelData::createDefaultFile() {
    std::ofstream file("leveldata.txt");
    if (!file) return;
    // 写入默认数据
    file << "15\n";                 // 总波数
    file << "0\n";                 // 僵尸类型（0=普通僵尸）
    file << "30,30,50,50,70,70,100,120,150,150,150,150,160,200,300\n";  // 每波权重
    file.close();
    std::cout << "Created default leveldata.txt" << std::endl;
}

void LevelData::applyTo(Spawn& spawn) {
    spawn.setSpawnWave(waveCount);
    spawn.setSpawnType(types);
    spawn.setSpawnWeight(weights);
}