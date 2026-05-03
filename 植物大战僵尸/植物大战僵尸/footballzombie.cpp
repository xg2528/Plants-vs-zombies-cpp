#include "pch.h"
#include "Zombies.h"
#include <cstdio>
#include "AudioManager.h"

extern void putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH);
extern AudioManager audio;

std::vector<IMAGE> FootballZombie::s_walk;
std::vector<IMAGE> FootballZombie::s_attack;
std::vector<IMAGE> FootballZombie::s_ornlost_walk;
std::vector<IMAGE> FootballZombie::s_ornlost_attack;
std::vector<IMAGE> FootballZombie::s_losthead_walk;
std::vector<IMAGE> FootballZombie::s_losthead_attack;
std::vector<IMAGE> FootballZombie::s_die;
bool FootballZombie::s_imagesLoaded = false;

static std::wstring buildP(int index, const std::wstring& dir) {
    std::wstring path = dir;
    if (index < 10) path += L"000" + std::to_wstring(index);
    else if (index < 100) path += L"00" + std::to_wstring(index);
    else path += L"0" + std::to_wstring(index);
    path += L".png";
    return path;
}

static void loadSet(std::vector<IMAGE>& imgs, int count, const std::wstring& dir) {
    for (int i = 1; i <= count; ++i) {
        IMAGE img;
        std::wstring path = buildP(i, dir);
        if (loadimage(&img, path.c_str()) != 0)
            printf("Failed: %S\n", path.c_str());
        imgs.push_back(img);
    }
}

void FootballZombie::loadSharedImages() {
    if (s_imagesLoaded) return;
    loadSet(s_walk, 11, L"resource/images/Zombies/FootballZombie/FootballZombie/frame_");
    loadSet(s_attack, 10, L"resource/images/Zombies/FootballZombie/FootballZombieAttack/frame_");
    loadSet(s_ornlost_walk, 11, L"resource/images/Zombies/FootballZombie/FootballZombieOrnLost/frame_");
    loadSet(s_ornlost_attack, 11, L"resource/images/Zombies/FootballZombie/FootballZombieOrnLostAttack/frame_");
    loadSet(s_losthead_walk, 10, L"resource/images/Zombies/FootballZombie/LostHead/frame_");
    loadSet(s_losthead_attack, 10, L"resource/images/Zombies/FootballZombie/LostHeadAttack/frame_");
    loadSet(s_die, 7, L"resource/images/Zombies/FootballZombie/Die/frame_");
    s_imagesLoaded = true;
    printf("FootballZombie images loaded\n");
}

FootballZombie::FootballZombie(int r, float startX, float startY) {
    row = r;
    worldX = startX;
    worldY = startY;
    hp = MAX_HP;
    stage = 0;
    dying = false;
    isAttacking = false;
    speed = 20.0f;  // double the minimum speed of NormalZombie
    currentFrame = 0;
    animTimer = 0.0f;
    animInterval = 0.1f;
    attackTimer = 0.0f;
    attackInterval = 0.65f;
    attackDamage = 60;
    deathTimer = 0.0f;
    deathFrame = 0;
    int p = rand() % 10;
    if (!p) sounds();
}

void FootballZombie::update(float delta) {
    if (dying) {
        deathTimer += delta;
        if (deathFrame < (int)s_die.size() - 1) {
            // Advance to next death frame
            if (deathTimer >= 0.25f) {
                deathTimer = 0.0f;
                deathFrame++;
            }
        } else {
            // Hold last frame for 0.5s, then done
            if (deathTimer >= 0.5f) {
                stage = 4;
            }
        }
        return;
    }

    // Auto-drain in headLost stage
    if (stage == 2) {
        hp -= (int)(15.0f * delta);
        if (hp <= 0) {
            hp = 0;
            dying = true;
            deathTimer = 0.0f;
            deathFrame = 0;
            return;
        }
    }

    // Update stage based on HP
    if (hp > HELMET_LOST_HP) stage = 0;
    else if (hp > HEAD_LOST_HP) stage = 1;
    else if (hp > 0) stage = 2;
    else { hp = 0; dying = true; deathTimer = 0.0f; deathFrame = 0; return; }

    if (!isAttacking) worldX -= speed * delta;

    std::vector<IMAGE>* frames = nullptr;
    switch (stage) {
    case 0: frames = isAttacking ? &s_attack : &s_walk; break;
    case 1: frames = isAttacking ? &s_ornlost_attack : &s_ornlost_walk; break;
    case 2: frames = isAttacking ? &s_losthead_attack : &s_losthead_walk; break;
    }
    if (frames && !frames->empty()) {
        animTimer += delta;
        if (animTimer >= animInterval) {
            animTimer -= animInterval;
            currentFrame = (currentFrame + 1) % (int)frames->size();
        }
    }
}

void FootballZombie::draw(int screenX, int screenY) {
    if (dying) {
        if (deathFrame < (int)s_die.size()) {
            const IMAGE& img = s_die[deathFrame];
            putimagePng(screenX, screenY, &img, 0, 0, img.getwidth(), img.getheight());
        }
        return;
    }
    std::vector<IMAGE>* frames = nullptr;
    switch (stage) {
    case 0: frames = isAttacking ? &s_attack : &s_walk; break;
    case 1: frames = isAttacking ? &s_ornlost_attack : &s_ornlost_walk; break;
    case 2: frames = isAttacking ? &s_losthead_attack : &s_losthead_walk; break;
    }
    if (frames && !frames->empty() && currentFrame < (int)frames->size()) {
        const IMAGE& img = (*frames)[currentFrame];
        putimagePng(screenX, screenY, &img, 0, 0, img.getwidth(), img.getheight());
    }
}

void FootballZombie::takeDamage(int damage) {
    if (dying) return;
    hp -= damage;
    if (hp < 0) hp = 0;
    if (hp == 0 && !dying) { dying = true; deathTimer = 0.0f; deathFrame = 0; }
}

bool FootballZombie::isDead() const { return stage == 4; }

void FootballZombie::drawDead() {}

void FootballZombie::sounds() {
    int id = rand() % 6 + 1;
    std::string file = "groan" + std::to_string(id) + ".mp3";
    audio.playEffect(file);
}
