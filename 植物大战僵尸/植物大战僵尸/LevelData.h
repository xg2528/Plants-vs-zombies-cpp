#pragma once
#include <vector>
#include "Spawn.h"

class LevelData {
public:
    LevelData();                       // 构造函数中自动读取文件
    void applyTo(Spawn& spawn);        // 将读取的数据应用到出怪管理器

private:
    int waveCount;                     // 总波数
    std::vector<int> types;            // 可生成的僵尸类型列表
    std::vector<int> weights;          // 每波总分阈值（长度等于 waveCount）
    void loadFromFile();               // 从 leveldata.txt 读取
    void createDefaultFile();          // 创建默认配置文件
};