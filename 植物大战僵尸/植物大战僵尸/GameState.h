#pragma once

class GameState
{
public:

	virtual ~GameState() = default;
	virtual void onEnter() = 0;        // 进入状态时调用
	virtual void onExit() = 0;         // 离开状态时调用
	virtual void handleInput() = 0;    // 处理输入
	virtual void update() = 0;         // 更新逻辑
	virtual void render() = 0;         // 绘制画面

private:

	




protected:



};