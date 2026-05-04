#include"pch.h"
#include"Menu.h"
#include"Playing.h"
#include"AudioManager.h"
#include"Game.h"
extern AudioManager audio;  // 假设 audio 是全局对象
// 在当前设备上绘制带透明通道的png图片的指定区域
// img_x, img_y   : 目标位置（窗口坐标）
// pSrcImg        : 源IMAGE对象指针
// srcX, srcY     : 源矩形左上角坐标（相对于源图片）
// srcW, srcH     : 源矩形宽度和高度
void putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH)
{
    // 1. 源图片有效性检查
    if (pSrcImg == nullptr) return;
    int src_width = pSrcImg->getwidth();
    int src_height = pSrcImg->getheight();
    if (src_width <= 0 || src_height <= 0) return;

    // 2. 目标窗口尺寸检查
    int dst_width = getwidth();
    int dst_height = getheight();
    if (dst_width <= 0 || dst_height <= 0) return;

    // 3. 源矩形裁剪
    if (srcX < 0) { srcW += srcX; srcX = 0; }
    if (srcY < 0) { srcH += srcY; srcY = 0; }
    if (srcX + srcW > src_width) srcW = src_width - srcX;
    if (srcY + srcH > src_height) srcH = src_height - srcY;
    if (srcW <= 0 || srcH <= 0) return;

    // 4. 目标位置裁剪
    int dstX = img_x;
    int dstY = img_y;
    int drawW = srcW;
    int drawH = srcH;
    int srcOffsetX = 0;
    int srcOffsetY = 0;
    if (dstX < 0) { drawW += dstX; srcOffsetX = -dstX; dstX = 0; }
    if (dstY < 0) { drawH += dstY; srcOffsetY = -dstY; dstY = 0; }
    if (dstX + drawW > dst_width) drawW = dst_width - dstX;
    if (dstY + drawH > dst_height) drawH = dst_height - dstY;
    if (drawW <= 0 || drawH <= 0) return;

    // 5. 防止异常超大绘制区域（避免长时间循环）
    const int MAX_DIM = 2000; // 通常图片不会大于2000x2000
    if (drawW > MAX_DIM || drawH > MAX_DIM) return;

    // 6. 获取缓冲区指针
    DWORD* dst = GetImageBuffer();
    DWORD* src = GetImageBuffer(const_cast<IMAGE*>(pSrcImg)); // 只读操作，const_cast 安全

    // 7. 定位到起始点
    src = src + (srcY + srcOffsetY) * src_width + (srcX + srcOffsetX);
    dst = dst + dstY * dst_width + dstX;

    // 8. 逐像素混合
    for (int i = 0; i < drawH; ++i) {
        for (int j = 0; j < drawW; ++j) {
            DWORD src_color = src[j];
            int src_a = (src_color >> 24) & 0xff;
            if (src_a == 0) continue;                     // 完全透明
            if (src_a == 255) { dst[j] = src_color; continue; } // 完全不透明
            int src_r = (src_color >> 16) & 0xff;
            int src_g = (src_color >> 8) & 0xff;
            int src_b = src_color & 0xff;
            DWORD dst_color = dst[j];
            int dst_r = (dst_color >> 16) & 0xff;
            int dst_g = (dst_color >> 8) & 0xff;
            int dst_b = dst_color & 0xff;
            // Alpha 混合
            int new_r = (src_r * src_a + dst_r * (255 - src_a)) / 255;
            int new_g = (src_g * src_a + dst_g * (255 - src_a)) / 255;
            int new_b = (src_b * src_a + dst_b * (255 - src_a)) / 255;
            dst[j] = (new_r << 16) | (new_g << 8) | new_b;
        }
        src += src_width;
        dst += dst_width;
    }
}//CSDN提供的函数，用于在当前设备上绘制带透明通道的png图片的指定区域，经过deepseek修改
Menu::Menu()
{
    loadimage(&surface, _T("resource/images/interface/Surface.png"));


    loadimage(&btnStart.img, _T("resource/images/interface/SelectorScreenStartAdventur.png"));
    btnStart.btnX = 475;   // 假设墓碑上按钮的 X 坐标
    btnStart.btnY = 60;   // 假设墓碑上按钮的 Y 坐标
    btnStart.id = 0; // 给开始按钮分配一个 ID
    buttons.push_back(btnStart);
	


    loadimage(&btnAlmanac.img, _T("resource/images/interface/SelectorScreen_Almanac.png"));
    btnAlmanac.btnX = 400;   // 假设图鉴按钮的 X 坐标
    btnAlmanac.btnY = 450;   // 假设图鉴按钮的 Y 坐标
    btnAlmanac.id = 1; // 给图鉴按钮分配一个 ID
    buttons.push_back(btnAlmanac);
	





}
void Menu::onEnter() {
    // 进入菜单界面时的初始化逻辑（如果有）
    for (auto& btn : buttons) {
        btn.isHover = false;// 初始化鼠标悬停状态
        btn.lastHover = false;
        btn.isPressed = false;
    }
    audio.playMusic("resource/audio/Faster.mp3");
}


void Menu::onExit() {
    // 离开菜单界面时的清理逻辑（如果有）
     audio.stopMusic(); // 停止背景音乐
}
void Menu::handleInput() {
    ExMessage msg;
    while (peekmessage(&msg, EM_MOUSE)) {
        if (msg.message == WM_MOUSEMOVE) {
            if (buttons.empty()) return;
            for (auto& btn : buttons) {
                if(btn.img.getwidth() == 0 || btn.img.getheight() == 0) continue;
                 int btnRight = btn.btnX + btn.img.getwidth();
                int btnBottom = btn.btnY + btn.img.getheight() / 2; // 热区高度取一半
                bool newHover = (msg.x >= btn.btnX && msg.x <= btnRight &&
                    msg.y >= btn.btnY && msg.y <= btnBottom);
                if (newHover != btn.isHover) {
                    btn.isHover = newHover;
                    // 当悬停状态从 false 变为 true 时播放音效
                    if (btn.isHover && !btn.lastHover) {
                        printf("Play hover sound\n");
                        // 需要访问 AudioManager 对象，可以全局 extern 或通过参数传递
                       
                        audio.playEffect("bleep.mp3");
                    }
                    btn.lastHover = btn.isHover;
                }
            }
        }
        if(msg.message == WM_LBUTTONDOWN) {
            for (auto& btn : buttons) {
                int btnRight = btn.btnX + btn.img.getwidth();
                int btnBottom = btn.btnY + btn.img.getheight() / 2; // 热区高度取一半
                if (msg.x >= btn.btnX && msg.x <= btnRight &&
                    msg.y >= btn.btnY && msg.y <= btnBottom) {
                    btn.isPressed = true;
                    printf("Button pressed at (%d, %d)\n", msg.x, msg.y);
                    // 在这里可以添加按钮被点击后的逻辑处理
                }
                if(btn.isPressed) {
                    // 按钮被按下后的逻辑处理
					audio.playEffect("gravebutton.mp3"); // 播放点击音效
                  
                }
            }
		}
        if (msg.message == WM_LBUTTONUP)
        {
            for (auto& btn : buttons) {
                if (btn.isPressed) {
                    btn.isPressed = false; // 重置按下状态
                    // 在这里可以添加按钮被释放后的逻辑处理
                    if (btn.id == 0) {
                        extern Game game;
                        game.changeState(std::make_unique<PlayingState>());
                    }
                    else if (btn.id == 1) {
                        // 图鉴暂未实现，可先输出提示
                        printf("Almanac not implemented yet.\n");
                    }
                    break;
                }
            }
        }
    }
}
void Menu::render() {
    putimage(0, 0, &surface);                     // 绘制背景
    for (auto& btn : buttons) {                   // 遍历所有按钮
        int halfHeight = btn.img.getheight() / 2;
        int offsetY = btn.isHover ? halfHeight : 0;
        // 临时计算绘制位置：按下时偏移2像素，否则不偏移
        int drawX = btn.btnX + (btn.isPressed ? 2 : 0);
        int drawY = btn.btnY + (btn.isPressed ? 2 : 0);
        putimagePng(drawX, drawY, &btn.img, 0, offsetY,
            btn.img.getwidth(), halfHeight);
    }
}
void Menu::update() {
    // 菜单界面通常不需要更新逻辑，可以留空
}