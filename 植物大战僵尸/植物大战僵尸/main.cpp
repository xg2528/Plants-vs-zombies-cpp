#include"pch.h" 
#include"Menu.h"
#include"AudioManager.h"
#include"GameState.h"
#include"Game.h"
#include<fstream>
#define WIDTH 900
#define HEIGHT 600

Menu menu;
AudioManager audio;
ExMessage msg;
Game game;
int main() {
  
    initgraph(WIDTH, HEIGHT, EX_SHOWCONSOLE);
    srand((unsigned)time(nullptr));//初始化随机数
    audio.loadAllEffects("resource/audio");
        game.run();
        return 0;
    

}




