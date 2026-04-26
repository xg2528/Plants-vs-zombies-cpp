#include"pch.h"
#include"Game.h"
#include"Menu.h"
const auto target_ms = std::chrono::milliseconds(1000) / 60;
void Game::changeState(std::unique_ptr<GameState> newState)//这就是虚函数的用途，实现多个接口的统一调用
{
    if (currentState) {
        currentState->onExit();
    }
    currentState = std::move(newState);
    if (currentState) {
        currentState->onEnter();
    }
}

void Game::run() {
    BeginBatchDraw();  // 开启双缓冲
    changeState(std::make_unique<Menu>());

    while (true) {
        auto start_time = std::chrono::steady_clock::now();//记录开始时间
        if (currentState) {
            currentState->handleInput();
            currentState->update();
            currentState->render();
        }
        auto end_time = std::chrono::steady_clock::now();//记录结束时间
        auto elapsed = end_time - start_time; //计算经过的时间
        if (elapsed < target_ms)
        {
            std::this_thread::sleep_for(target_ms - elapsed);
        }
        FlushBatchDraw();  // 将缓冲区内容绘制到屏
    }

    EndBatchDraw();  // 理论上不会执行，但为了完整性保留
}