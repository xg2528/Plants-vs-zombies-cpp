#pragma once
#include<vector>
#include"Constants.h"
#include"Menu.h"

class Plants
{
public:
	
	virtual void setHp(int hp) = 0;
	virtual void loadImgs() = 0;
	virtual void update(float delta) = 0;
	virtual void draw(int x, int y) = 0;    // 新增：绘制植物
	virtual void attack(float delta, const std::vector<std::unique_ptr<Zombies>>& zombies) = 0;
	virtual void takeDamage(int damage) = 0;
	virtual bool isDead() const = 0;
	Plants(int num) : m_hp(300),m_pngNum(num), row(-1), col(-1), type(-1) {}
	Plants() :m_hp(300), m_pngNum(12), row(-1), col(-1), type(-1) {}
	int row, col;   // 网格行列
	int type;       // 0=豌豆射手, 1=向日葵...

protected:
	int m_hp;
	std::vector<IMAGE>Img;
	int m_pngNum;
	
	

};

class Peashooter :public Plants
{

public:


	
	void setHp(int hp)override { m_hp = hp; };
    void loadImgs()override;
	Peashooter() : Plants(12), shootTimer(0.0f), shootInterval(1.0f), currentFrame(0), animTimer(0.0f) {
		loadImgs();
	}
	void update(float delta)override;
	void draw(int x, int y) override;    // 实现
	void attack(float delta, const std::vector<std::unique_ptr<Zombies>>& zombies)override;
	Peashooter(int r, int c);            // 构造函数接收行列
	void takeDamage(int damage)override {
		m_hp -= damage;
		if (m_hp <= 0) m_hp = 0;
	}
	bool isDead()const override { return m_hp <= 0; }
	
	
private:

	float shootTimer;        // 距离下次发射的剩余时间（秒）
	float shootInterval;     // 发射间隔（秒）
	int currentFrame;        // 当前动画帧索引
	float animTimer;         // 动画帧切换计时器
	float animInterval;   // 每帧动画间隔（例如 0.1 秒）



};

class Sunflower : public Plants {
public:
	Sunflower(int row, int col);                  // 构造函数（指定行列）
	void update(float delta) override;
	void draw(int x, int y) override;
	void takeDamage(int damage) override;
	bool isDead() const override;
	void loadImgs() override;
	void setHp(int hp) override { m_hp = hp; }
	void attack(float delta, const std::vector<std::unique_ptr<Zombies>>& zombies) override;

private:
	static const int PRODUCE_INTERVAL = 10000;   // 生产阳光间隔（毫秒）
	float produceTimer;                          // 生产计时器（毫秒）
	bool isProducing;                            // 是否正在生产阳光（用于切换动画）
	float produceAnimTimer;                      // 生产动画停留计时
	int currentFrame;
	float animTimer;
	float animInterval;
	static const int NORMAL_OFFSET = 0;          // 普通状态下源Y偏移
	static const int PRODUCE_OFFSET = 0;          // 生产状态下源Y偏移（下半部分）
	// 实际使用时，图片高度的一半作为偏移
	int currentInterval;

};

class Repeater : public Plants {
public:
	void setHp(int hp)override { m_hp = hp; };
	void loadImgs()override;
	Repeater() : Plants(15), shootTimer(0.0f), shootInterval(1.8f), secondPeaTimer(-1.0f), fireSecondPea(false), currentFrame(0), animTimer(0.0f) {
		loadImgs();
	}
	void update(float delta)override;
	void draw(int x, int y) override;
	void attack(float delta, const std::vector<std::unique_ptr<Zombies>>& zombies)override;
	Repeater(int r, int c);
	void takeDamage(int damage)override {
		m_hp -= damage;
		if (m_hp <= 0) m_hp = 0;
	}
	bool isDead()const override { return m_hp <= 0; }
private:
	float shootTimer;
	float shootInterval;
	float secondPeaTimer;   // delay before second pea, -1 = inactive
	bool fireSecondPea;     // true when second pea ready to fire
	int currentFrame;
	float animTimer;
	float animInterval;
};