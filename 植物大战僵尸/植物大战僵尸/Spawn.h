#pragma once

#include<vector>

#include"Zombies.h"

class Spawn

{

public:





	Spawn()//为了直观，就直接把初始化写在函数里，不用初始化构造列表了

	{

		wave = 10;

		frequency = 5;

		spawnType = { 0 };

		spawnWeight.assign(wave, 100); // 默认每波权重为100，实际使用时可以根据需要调整



	}

	Spawn(int wave)

	{

		this->wave = wave;

		this->frequency = 5;

		spawnType = { 0 };

		spawnWeight.assign(this->wave, 100); // 默认每波权重为100，实际使用时可以根据需要调整

	}

	Spawn(int wave, int frequency)

	{

		this->wave = wave;

		this->frequency = frequency;

		spawnType = { 0 };

		spawnWeight.assign(this->wave, 100); // 默认每波权重为100，实际使用时可以根据需要调整

	}





	void setSpawnType(const std::vector<int>& types) {

		spawnType = types;

	}

	void setSpawnWeight(const std::vector<int>& weights) {

		if (weights.size() == wave) {

			spawnWeight = weights;

		}

	}

	void setSpawnWave(int wave)

	{

		this->wave = wave;

	}

	std::vector<std::unique_ptr<Zombies>>& getZombies()

	{

		return spawnFinal;

	}

	void update(float delta);

		// 这里可以实现每隔 frequency 秒生成一波僵尸的逻辑

		// 根据 spawnType 和 spawnWeight 来决定生成哪些类型的僵尸

		// 例如，可以使用 spawnWeight 来随机选择生成哪一波的僵尸

	



	//废弃void render() {

		// 这里可以实现生成的僵尸的绘制逻辑

	//}





	int currentWave=-1;

	int currentWeight = 0;

	int getWave()

	{

		return wave;

	}

	const std::vector<int>& getSpawnTypes() const { return spawnType; }

	const std::vector<int>& getSpawnWeights() const { return spawnWeight; }













private:



	int wave;//总波数

	int frequency;//生成频率（秒）

	float timer=0; // 计时器，用于跟踪生成时间

	std::vector<int> spawnType; // 储存生成僵尸的种类

	std::vector<int> spawnWeight;  //储存部署每一波僵尸的权重，每一波僵尸的权重加起来要等于这个

	std::vector<std::unique_ptr<Zombies>> spawnFinal; //得到最后生成僵尸的数量和种类，可以在update函数里根据spawnType和spawnWeight计算得到





};