#include "pch.h"
#include "Playing.h"
#include <cmath>
#include "AudioManager.h" // 如果需要播放音乐
#include "Plants.h"
#include "Sun.h"
#include"Zombies.h"
#include"Spawn.h"
#include"Constants.h"
void checkBulletCollision(std::vector<std::unique_ptr<Zombies>>& zombies);
void checkPlantCollision(std::vector<std::unique_ptr<Zombies>>& zombies,
    Plants* gridPlants[GRID_ROWS][GRID_COLS],
    float delta);
void checkWin(std::vector<std::unique_ptr<Zombies>>& zombies);
extern void  putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH);
extern AudioManager audio;
std::vector<std::unique_ptr<Bullet>> g_bullets;


// 成员变量
Plants* gridPlants[GRID_ROWS][GRID_COLS];           // 用于快速判断占用
std::vector<std::unique_ptr<Plants>> plantsList;    // 存储所有植物
std::vector<Sun> suns;//阳光列表





void PlayingState::onEnter() {
    //初始化植物占用数组
    memset(gridPlants, 0, sizeof(gridPlants));
    // 加载游戏背景图片（假设有一张 background.png）
    loadimage(&background, _T("resource/images/interface/background1.jpg"));
    loadimage(&sunBack, _T("resource/images/interface/SunBack.png"));
    // 播放游戏背景音乐（例如 "grasswalk.mp3"）
    cameraX = 0.0f;// 初始化滚动变量
    scrollState = 0;           // 0=初始停留
	currentMusic = 0;         // 0=无音乐
    //phaseStartTime = std::chrono::steady_clock::now();
    currentSun = 150;



    spawn = Spawn(10, 20);  // 总波数10，每20秒一波
    // 设置每波总分阈值（假设 wave 为10）
    std::vector<int> weights = { 100, 110, 120, 110,70,10,60,10,60,30 }; // 每波阈值
    spawn.setSpawnWeight(weights);
    // 设置可生成的僵尸类型
    spawn.setSpawnType({ 0 });  // 只有普通僵尸

    //读取出怪
    levelData.applyTo(spawn);
	// ===== Calculate expected zombie composition for preview =====
	{
		const auto& types = spawn.getSpawnTypes();
		const auto& weights = spawn.getSpawnWeights();

		// Get cost weight for each zombie type
		auto getCost = [](int type) -> int {
			switch (type) {
			case 0: return 10;   // NormalZombie
			case 1: return 20;   // FlagZombie
			default: return 10;
			}
		};

		// Get preview image path for each zombie type
		auto getPreviewPath = [](int type) -> std::wstring {
			switch (type) {
			case 0: return L"resource/images/extra/Zombie/frame_0001.png";
			case 1: return L"resource/images/Card/Zombies/FlagZombie.png";
			default: return L"";
			}
		};

		// Calculate expected spawn count for each type
		// With uniform random selection: expected = totalWeight / sum(costs)
		int totalCostSum = 0;
		for (int t : types) totalCostSum += getCost(t);
		int totalWeight = 0;
		for (int w : weights) totalWeight += w;

		std::map<int, float> expectedCount;
		for (int t : types) {
			expectedCount[t] = (float)totalWeight / totalCostSum;
		}

		// Scale to preview slots (max 10) proportionally
		const int MAX_PREVIEW = 10;
		float totalExpected = 0;
		for (auto& p : expectedCount) totalExpected += p.second;

		previewImages.clear();
		previewZombies.clear();
		int slotIndex = 0;

		for (auto& [type, expected] : expectedCount) {
			int displayCount = (int)(std::max)(1.0f, std::round(expected / totalExpected * MAX_PREVIEW));
			if (slotIndex + displayCount > MAX_PREVIEW)
				displayCount = MAX_PREVIEW - slotIndex;
			if (displayCount <= 0) break;

			// Load preview image for this zombie type
			IMAGE img;
			std::wstring path = getPreviewPath(type);
			if (path.empty()) continue;
			if (loadimage(&img, path.c_str()) != 0) continue;
			previewImages.push_back(img);
			IMAGE* imgPtr = &previewImages.back();

			// Arrange in 2-column grid on the right side of the lawn
			for (int i = 0; i < displayCount; i++) {
				int totalSlot = slotIndex + i;
				int col = totalSlot % 2;  // left or right column
				int row = totalSlot / 2;  // row 0~4

				PreviewZombie pz;
				pz.img = imgPtr;
				pz.worldX = (float)(GRID_WORLD_X + GRID_COLS * CELL_W + 60 + col * 110);
				pz.worldY = GRID_WORLD_Y + row * CELL_H + (CELL_H - img.getheight()) / 2 - 10;
				previewZombies.push_back(pz);
			}
			slotIndex += displayCount;
		}
	}



    //加载普通僵尸
    NormalZombie::loadSharedImages();





    // 豌豆射手卡片
    Card pea;
    loadimage(&pea.img, _T("resource/images/Card/Plants/Peashooter.png"));//        
    loadimage(&pea.plantImg, _T("resource/images/Plants/Peashooter/frame_0001.png")); // 静态图片
    pea.x = 200;
    pea.y = 10;
    pea.cost = 100;
	pea.totalCoolTime = 5000.0f;//冷却时间5秒
    pea.plantType = 0;
    pea.isSelected = false;
    pea.isCooling = false;
    pea.coolingRemain = 0;
    cards.push_back(pea);

    // 向日葵卡片
    Card sunflower;
    loadimage(&sunflower.img, _T("resource/images/Card/Plants/SunFlower.png"));
    loadimage(&sunflower.plantImg, _T("resource/images/Plants/SunFlower/0.gif")); // 静态图片
    sunflower.x = 300;
    sunflower.y = 10;
    sunflower.cost = 50;
    sunflower.totalCoolTime = 4000.0f;   // 冷却4秒
    sunflower.plantType = 1;             // 植物类型1
    cards.push_back(sunflower);

}

void PlayingState::onExit() {
    audio.stopMusic();

    // 释放所有植物对象（unique_ptr 自动 delete）
    plantsList.clear();

    // 清空网格指针数组，避免下次进入时残留野指针
    for (int i = 0; i < GRID_ROWS; ++i) {
        for (int j = 0; j < GRID_COLS; ++j) {
            gridPlants[i][j] = nullptr;
        }
    }
	//清空所有阳光对象
	suns.clear();
    g_bullets.clear();
    // 可选：重置其他状态
    currentSun = 150;
    for (auto& c : cards) c.isSelected = false;
	previewZombies.clear();  // clear preview data
	previewImages.clear();
}
void PlayingState::handleInput() {
    ExMessage msg;
    // 处理键盘（ESC 返回菜单）
    while (peekmessage(&msg, EM_KEY | EM_MOUSE)) {


        if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
            extern Game game;
            game.changeState(std::make_unique<Menu>());
        }
        // 处理鼠标右键：取消所有卡片选中
        if (msg.message == WM_RBUTTONDOWN)
        {
            for (auto& c : cards) c.isSelected = false;
            printf("Right click: cleared selection\n");
        }
        // 处理鼠标
        if (msg.message == WM_LBUTTONDOWN) {
            //检测是否点击了阳光
            for (auto& sun : suns)
            {
                const IMAGE& img = sun.getFirstFrame();
                int screenX = sun.worldX - (int)cameraX;
                int screenY = sun.worldY;
                int sunRight = screenX + img.getwidth();
                int sunBottom = screenY + img.getheight();
                if (msg.x >= screenX && msg.x <= sunRight &&
                    msg.y >= screenY&& msg.y <= sunBottom && scrollState == 5) {
					currentSun += 25; // 增加阳光
					sun.isCollected = true; // 标记为已收集
                    audio.playEffect("points.mp3");
					break; // 只处理一个阳光
                 
                }
                



            }
















            bool hitCard = false;
            // 先检查是否点击了卡片
            for (size_t i = 0; i < cards.size(); ++i) {
                const auto& card = cards[i];
                int cardRight = card.x + card.img.getwidth();
                int cardBottom = card.y + card.img.getheight() / 2;
                // 调试输出：打印卡片区域和鼠标坐标
                printf("Card %zu: x=%d, y=%d, w=%d, h=%d, right=%d, bottom=%d\n",
                    i, card.x, card.y, card.img.getwidth(), card.img.getheight(),
                    cardRight, cardBottom);
                printf("Mouse: (%d, %d)\n", msg.x, msg.y);
                if (msg.x >= card.x && msg.x <= cardRight &&
                    msg.y >= card.y && msg.y <= cardBottom && scrollState == 5) {
                    // 取消所有卡片的选中状态
                    for (auto& c : cards) c.isSelected = false;
                    if (!card.isCooling)
                    {
                        cards[i].isSelected = true;
                        audio.playEffect("seedlift.mp3");
                        printf("Card selected! cost=%d\n", cards[i].cost);
                        hitCard = true;
                        
                    }
                    else {
                        audio.playEffect("buzzer.mp3");
                        printf("Card is cooling! cost=%d\n", cards[i].cost);

                    }
					break; // 只处理一个卡片
                }
            }
            if (!hitCard) {
                printf("Try to plant at (%d, %d)\n", msg.x, msg.y);
                // 种植逻辑：如果有选中的卡片且滚动结束，且点击位置在网格内，且阳光足够，且格子为空
                if (scrollState == 5) {
                    // 查找当前选中的卡片
                    int selectedIdx = -1;
                    for (size_t i = 0; i < cards.size(); ++i) {
                        if (cards[i].isSelected) {
                            selectedIdx = i;
                            break;
                        }
                    }
                    if (selectedIdx != -1) {
                        Card& card = cards[selectedIdx];
                        // 屏幕坐标转世界坐标
                        int screenX = msg.x;
                        int screenY = msg.y;
                        int worldX = screenX + (int)cameraX;
                        int worldY = screenY;
                        int col = (worldX - GRID_WORLD_X) / CELL_W;
                        int row = (worldY - GRID_WORLD_Y) / CELL_H;
                        if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS) {
                            if (gridPlants[row][col] == nullptr && currentSun >= card.cost) {
                                Plants* plant = nullptr;
                                if (card.plantType == 0) { // 豌豆射手
                                    plant = new Peashooter(row, col);
                                }
                                else if (card.plantType == 1) {
                                    plant = new Sunflower(row, col);
                                }
                                // 其他类型后续添加
                                if (plant) {
                                    gridPlants[row][col] = plant;
                                    plantsList.push_back(std::unique_ptr<Plants>(plant));
                                    currentSun -= card.cost;
                                    card.isSelected = false; // 种完后取消选中
									card.isCooling = true; // 开始冷却
                                    audio.playEffect("plant1.mp3");
                                }
                            }
                        }
                    }
                }
            }
        }
        // 注意：WM_LBUTTONUP 不再处理种植，只做可能的清理（例如取消选中，但此处留空）
        // 如果你希望在草坪上松开左键时也做些什么，可以在这里添加，但原版是按下时种植，所以这里不需要。
    }
}
void PlayingState::update() {
    
    // 滚动速度：每帧移动 4 像素
    const float step = 4.0f;
    // 停留帧数：约 240 帧（假设 60帧/秒，4秒 ≈ 240帧）
    const int waitFrames = 240;

    static int frameCounter = 0;  // 用于停留计数

    switch (scrollState) {
    case 0: // 初始停留
       
        frameCounter++;
        if (currentMusic != 1) {
            audio.playMusic("resource/audio/Look up at the Sky.mp3"); // 循环
            currentMusic = 1;
        }
        if (frameCounter >= waitFrames) {
            scrollState = 1;
            frameCounter = 0;
            // 开始向右滚动
            
        }
        break;

    case 1: // 向右滚动
        cameraX += step;
        if (cameraX >= 500) {
            cameraX = 500;
            scrollState = 2;
            // 到达右侧
        }
        break;

    case 2: // 右停留
        frameCounter++;
        if (frameCounter >= waitFrames) {
            scrollState = 3;
            frameCounter = 0;
            // 开始向左滚动，继续播放 lookatthesky（已经播放中，无需切换）
        }
        break;

    case 3: // 向左滚动
        cameraX -= step;
        if (cameraX <= 150) {
            cameraX = 150;
            scrollState = 4;
            
        }
        break;

    case 4:// 左停留

        frameCounter++;
        if (frameCounter >= waitFrames) {
            scrollState = 5;
            frameCounter = 0;
            // 
        }
        break;
    case 5: // 滚动结束
        // 检查 readysetplants 是否播放完毕，若完毕则切换到 UraniwaNi
        // 注意：由于 playMusic 是异步的，我们需要定时检查是否还在播放
        // 简单起见，可以在 update 中每帧检测，或者设定一个定时器延迟切换
        // 这里用一个简单方法：记录播放 readysetplants 的开始时间，3秒后切换
        // 滚动完成，播放 readysetplants（一次性）
        if (currentMusic != 2&&currentMusic!=3) {
            audio.playMusic("resource/audio/readysetplant.mp3", false);
            currentMusic = 2;
        }
        static std::chrono::steady_clock::time_point plantStartTime;
        if (currentMusic == 2) {
            if (plantStartTime.time_since_epoch().count() == 0) {
                plantStartTime = std::chrono::steady_clock::now();
            }
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - plantStartTime).count();
            if (elapsed > 5000) {
               audio.playMusic("resource/audio/UraniwaNi.mp3", true);
                currentMusic = 3;
                plantStartTime = std::chrono::steady_clock::time_point(); // 重置为零
            }
        }
        break;
    }
    static auto lastTime = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    float delta = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;

    for (auto& plantPtr : plantsList) {
        plantPtr->update(delta);
    }
    for (auto& sun : suns)
    {
        sun.update(delta);
    }

    // 在 update 函数末尾，滚动结束后生成阳光
    static float sunSpawnTimer = 0.0f;
    if (scrollState == 5) {
        sunSpawnTimer += delta;
		if (sunSpawnTimer >= 5.0f) { // 每5秒掉落一个阳光,正式游戏时应该改成12秒
            sunSpawnTimer = 0.0f;
            suns.emplace_back(); // 创建新阳光（构造函数随机位置）
        }
    }

	//更新卡片冷却状态
    for (auto& card : cards) {
        if (card.isCooling) {
            card.coolingRemain += delta * 1000; // delta 是秒，转换为毫秒
            if (card.coolingRemain >= card.totalCoolTime) {
                card.coolingRemain = 0;
                card.isCooling = false; // 冷却结束
            }
        }

    }


     spawn.update(delta);  // 更新出怪计时器，生成新僵尸
    // 更新所有僵尸（通过 spawn.getZombies()）
    for (auto& zombie : spawn.getZombies()) {
        zombie->update(delta);
    }
    // 移除死亡僵尸（放在攻击之前）
    auto& zombies = spawn.getZombies();
    zombies.erase(std::remove_if(zombies.begin(), zombies.end(),
        [](const std::unique_ptr<Zombies>& z) { return z->isDead(); }), zombies.end());
    //移除死亡植物
    // 手动遍历以确保同时清理 gridPlants
    auto it = plantsList.begin();
    while (it != plantsList.end()) {
        if ((*it)->isDead()) {
            int r = (*it)->row;
            int c = (*it)->col;
            if (gridPlants[r][c] == it->get())   // 避免误清
                gridPlants[r][c] = nullptr;
            it = plantsList.erase(it);
        }
        else {
            ++it;
        }
    }


    // 单独处理攻击：遍历所有植物和僵尸，检测碰撞并发射子弹,如果不能攻击的植物就不用调用了
    for (auto& plantPtr : plantsList) {
        switch (plantPtr->type)
        {
        case 0:
            plantPtr->attack(delta, spawn.getZombies());
            break;


        }
        
        
    }

    // 更新所有子弹
    for (auto& bullet : g_bullets) {
        bullet->update(delta);
    }
    // 移除超出边界的子弹
    g_bullets.erase(std::remove_if(g_bullets.begin(), g_bullets.end(),
        [](const std::unique_ptr<Bullet>& b) { return !b->isActive(); }), g_bullets.end());

    // 子弹与僵尸碰撞检测
    checkBulletCollision(spawn.getZombies());
    //植物与僵尸的检测
    checkPlantCollision(spawn.getZombies(), gridPlants, delta);
    //游戏胜利检测
    auto& zomList = spawn.getZombies();
    if (spawn.currentWave >= spawn.getWave() && zomList.empty())
    {
        audio.stopMusic();
        audio.playEffect("winmusic.mp3");
        MessageBox(GetHWnd(), _T("你赢了！"), _T("植物大战僵尸"), MB_OK);
        extern Game game;
        game.changeState(std::make_unique<Menu>());
    }
    //游戏失败检测
    checkWin(spawn.getZombies());







}

void PlayingState::render() {

    cleardevice();  // 清屏
    // 背景图片宽度 1400，窗口宽度 900
    // cameraX 的取值范围 0 到 (1400 - 900) = 500
    // 绘制时让图片左150位于 -cameraX，这样显示的区域就是从 cameraX 开始的 900 像素宽
    int x = -(int)(cameraX);
    // 绘制背景
    putimage(x, 0, &background);
    // 可以绘制一些文字提示
    // 绘制阳光背景
    putimage(10, 10, &sunBack);   // 假设阳光背景位置 (10,10)
    // 在背景上绘制阳光数值（黑色文字）
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);  // 文字背景透明
    TCHAR sunText[32];
    _stprintf_s(sunText, _T("%d"), currentSun);
    outtextxy(10 + (sunBack.getwidth() - textwidth(sunText)) / 2,
        10 + (sunBack.getheight() - textheight(sunText)) / 2,
        sunText);





    for (auto& card : cards) {
        int cardH = card.img.getheight() / 2;  // 卡片图片高度的一半
        // 绘制卡片上半部分（正常状态）
        int OffestY = card.isCooling || card.isSelected ? cardH : 0;
        putimagePng(card.x, card.y, &card.img, 0, OffestY, card.img.getwidth(), cardH);
        // 如果有高亮等，继续添加

		// 绘制冷却时间（如果正在冷却）
        if (card.isCooling) {
            settextcolor(BLACK);
            setbkmode(TRANSPARENT);  // 文字背景透明
            TCHAR coolingText[32];
            _stprintf_s(coolingText, _T("%.2f"), (card.totalCoolTime-card.coolingRemain)/1000.0f);
            outtextxy(card.x + 15, card.y + 25, coolingText);
        }





        //绘制卡片的价格
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);  // 文字背景透明
        TCHAR costText[32];
        _stprintf_s(costText, _T("%d"), card.cost);
        outtextxy(card.x + 65, card.y + 25, costText);
    }

    // 绘制被选中的植物（跟随鼠标，移到循环外部）
    for (const auto& card : cards) {
        if (card.isSelected) {
            // 使用非阻塞方式获取鼠标位置
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(GetHWnd(), &pt);
            int imgW = card.plantImg.getwidth();
            int imgH = card.plantImg.getheight();
            // 使图片中心对准鼠标
            putimagePng(pt.x - imgW / 2, pt.y - imgH / 2, &card.plantImg, 0, 0, imgW, imgH);
            break;
        }
    }
	// Draw preview zombies during intro camera scroll
	if ((scrollState >= 1 && scrollState <= 3) && !previewZombies.empty()) {
		for (auto& pz : previewZombies) {
			int screenX = (int)(pz.worldX - cameraX);
			int screenY = (int)pz.worldY;
			putimagePng(screenX, screenY, pz.img, 0, 0, pz.img->getwidth(), pz.img->getheight());
		}
	}
    // 绘制所有植物
    for (auto& plantPtr : plantsList) {
        int screenX = GRID_WORLD_X + plantPtr->col * CELL_W - (int)cameraX;
        int screenY = GRID_WORLD_Y + plantPtr->row * CELL_H;
        plantPtr->draw(screenX, screenY);
    }
	// 绘制所有阳光，并移除已收集的阳光
    for (auto it = suns.begin(); it != suns.end(); ) {
        if (it->isCollected) {
            it = suns.erase(it);   // erase 返回下一个有效迭代器，不需要手动 ++
        }
        else {
            ++it;                 // 不删除时才手动前进
        }
    }
    //绘制阳光testing
    for (const auto& sun : suns)
    {
      
        sun.draw((int)cameraX);
    }



    // 绘制僵尸（按行号升序排序，使下面行的僵尸后绘制，从而遮挡上面行的僵尸）
    auto& zombies = spawn.getZombies();
    if (!zombies.empty()) {
        std::vector<Zombies*> sorted;
        sorted.reserve(zombies.size());
        for (auto& z : zombies) sorted.push_back(z.get());
        std::sort(sorted.begin(), sorted.end(),
            [](const Zombies* a, const Zombies* b) { return a->row < b->row; });
        for (Zombies* zombie : sorted) {
            int screenX = (int)(zombie->worldX - cameraX);
            int screenY = (int)zombie->worldY;
            if (!zombie->isDead())
            {
                zombie->draw(screenX, screenY);
            }
            else {

                zombie->drawDead();
            }
        }
    }


    for (auto& bullet : g_bullets) {
        bullet->draw((int)cameraX);
    }
}


// 子弹与僵尸碰撞检测
void checkBulletCollision(std::vector<std::unique_ptr<Zombies>>& zombies) {
    for (auto it = g_bullets.begin(); it != g_bullets.end(); ) {
        Bullet* bullet = it->get();
        if (!bullet->isActive() || !bullet->canHit) {
            ++it;
            continue;
        }
        bool hit = false;
        for (auto& zombie : zombies) {
            if (zombie->isDead()) continue;
            if (bullet->getRow() != zombie->row) continue;
            float zombieLeft = zombie->worldX;
            float bulletRight = bullet->getWorldX();
            // 简单碰撞：子弹进入僵尸大约一半矩形范围
            if (bulletRight > zombieLeft + 20 && bulletRight < zombieLeft + 60) {
                zombie->takeDamage(bullet->getDamage());
                hit = true;
                audio.playEffect("splat1.mp3");
                break;
            }
        }
        if (hit) it = g_bullets.erase(it);
        else ++it;
    }
}
// 僵尸与植物碰撞检测
void checkPlantCollision(std::vector<std::unique_ptr<Zombies>>& zombies,
    Plants* gridPlants[GRID_ROWS][GRID_COLS],
    float delta) {
    for (auto& zombie : zombies) {
        // 计算僵尸当前所在列（基于世界坐标，取整）
        int col = (int)((zombie->worldX + ZOMBIE_COLLIDE_OFFSET - GRID_WORLD_X) / CELL_W);
        if (col < 0 || col >= GRID_COLS) {
            zombie->isAttacking = false;
            continue;
        }
        int row = zombie->row;
        Plants* plant = gridPlants[row][col];
        if (plant != nullptr && !plant->isDead()) {
            // 僵尸与植物在同一格，进入攻击状态
            zombie->isAttacking = true;
            // 攻击冷却
            if (zombie->attackTimer <= 0.0f) {
                plant->takeDamage(zombie->attackDamage);
                zombie->attackTimer = zombie->attackInterval;
                audio.playEffect("chompsoft.mp3");
                // 如果植物死亡，标记网格为空（植物对象稍后统一清理）
                if (plant->isDead()) {
                    audio.playEffect("gulp.mp3");
                    gridPlants[row][col] = nullptr;
                }
            }
            else {
                zombie->attackTimer -= delta;
            }
        }
        else {
            zombie->isAttacking = false;
        }
    }
}
void checkWin(std::vector<std::unique_ptr<Zombies>>& zombies) {
    static bool gameLost = false;   // 确保只触发一次
    if (gameLost) return;

    for (auto& zom : zombies) {
        // 阈值根据实际场景调整
        if (zom->worldX < GRID_WORLD_X - (CELL_W *3)/2) {
            gameLost = true;
            audio.stopMusic();
            audio.playEffect("losemusic.mp3");
            MessageBox(GetHWnd(), _T("僵尸吃掉了你的脑子！"), _T("植物大战僵尸"), MB_OK);
            extern Game game;
            game.changeState(std::make_unique<Menu>());
            return;
        }
    }
}