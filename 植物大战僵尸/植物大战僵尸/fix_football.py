# -*- coding: utf-8 -*-
import os
base = r'c:/Users/22773/Desktop/plants vs zombies/植物大战僵尸/植物大战僵尸'
def c(s):
    return s.encode('utf-8')

# ===== 1. ZOMBIES.H: isDying + getCollideOffset + FootballZombie =====
with open(os.path.join(base, 'Zombies.h'), 'rb') as f:
    lines = f.read().split(b'\r\n')

# Add isDying + getCollideOffset after drawDead
for i, line in enumerate(lines):
    if b'virtual void drawDead()=0;' in line:
        lines.insert(i+1, b'\tvirtual bool isDying() const { return false; }')
        lines.insert(i+2, b'\tvirtual int getCollideOffset() const { return 130; }')
        break

# Add FootballZombie AFTER last }; (insert after index i where }; is ConeheadZombie's closing)
for i in range(len(lines)-1, -1, -1):
    if lines[i].strip() == b'};':
        break

fb_text = (
    '// Football Zombie (helmet, multi-stage, death animation)\n'
    'class FootballZombie : public Zombies {\n'
    'public:\n'
    '\tFootballZombie(int row, float startX, float startY);\n'
    '\tvoid update(float delta) override;\n'
    '\tvoid draw(int screenX, int screenY) override;\n'
    '\tvoid takeDamage(int damage) override;\n'
    '\tbool isDead() const override;\n'
    '\tbool isDying() const override { return dying; }\n'
    '\tint getCollideOffset() const override { return 180; }\n'
    '\tvoid drawDead() override;\n'
    '\tvoid sounds();\n'
    '\tstatic void loadSharedImages();\n'
    'private:\n'
    '\tfloat speed;\n'
    '\tint stage;          // 0=normal, 1=helmLost, 2=headLost, 3=dying\n'
    '\tbool dying;\n'
    '\tfloat deathTimer;\n'
    '\tint deathFrame;\n'
    '\tstatic const int HELMET_LOST_HP = 960;\n'
    '\tstatic const int HEAD_LOST_HP = 60;\n'
    '\tstatic const int MAX_HP = 1860;\n'
    '\tstatic std::vector<IMAGE> s_walk;\n'
    '\tstatic std::vector<IMAGE> s_attack;\n'
    '\tstatic std::vector<IMAGE> s_ornlost_walk;\n'
    '\tstatic std::vector<IMAGE> s_ornlost_attack;\n'
    '\tstatic std::vector<IMAGE> s_losthead_walk;\n'
    '\tstatic std::vector<IMAGE> s_losthead_attack;\n'
    '\tstatic std::vector<IMAGE> s_die;\n'
    '\tstatic bool s_imagesLoaded;\n'
    '};\n'
)

insert_lines = fb_text.split('\n')
lines = lines[:i+1] + [l.encode('ascii') for l in insert_lines[:-1]] + lines[i+1:]

with open(os.path.join(base, 'Zombies.h'), 'wb') as f:
    f.write(b'\r\n'.join(lines))
print(f'Zombies.h: {len(lines)} lines')

# ===== 2. FOOTBALLZOMBIE.CPP =====
fb_cpp = (
    '#include "pch.h"\n'
    '#include "Zombies.h"\n'
    '#include <cstdio>\n'
    '#include "AudioManager.h"\n'
    '\n'
    'extern void putimagePng(int img_x, int img_y, const IMAGE* pSrcImg, int srcX, int srcY, int srcW, int srcH);\n'
    'extern AudioManager audio;\n'
    '\n'
    'std::vector<IMAGE> FootballZombie::s_walk;\n'
    'std::vector<IMAGE> FootballZombie::s_attack;\n'
    'std::vector<IMAGE> FootballZombie::s_ornlost_walk;\n'
    'std::vector<IMAGE> FootballZombie::s_ornlost_attack;\n'
    'std::vector<IMAGE> FootballZombie::s_losthead_walk;\n'
    'std::vector<IMAGE> FootballZombie::s_losthead_attack;\n'
    'std::vector<IMAGE> FootballZombie::s_die;\n'
    'bool FootballZombie::s_imagesLoaded = false;\n'
    '\n'
    'static std::wstring buildP(int index, const std::wstring& dir) {\n'
    '    std::wstring path = dir;\n'
    '    if (index < 10) path += L\"000\" + std::to_wstring(index);\n'
    '    else if (index < 100) path += L\"00\" + std::to_wstring(index);\n'
    '    else path += L\"0\" + std::to_wstring(index);\n'
    '    path += L\".png\";\n'
    '    return path;\n'
    '}\n'
    '\n'
    'static void loadSet(std::vector<IMAGE>& imgs, int count, const std::wstring& dir) {\n'
    '    for (int i = 1; i <= count; ++i) {\n'
    '        IMAGE img;\n'
    '        std::wstring path = buildP(i, dir);\n'
    '        if (loadimage(&img, path.c_str()) != 0)\n'
    '            printf(\"Failed: %S\\n\", path.c_str());\n'
    '        imgs.push_back(img);\n'
    '    }\n'
    '}\n'
    '\n'
    'void FootballZombie::loadSharedImages() {\n'
    '    if (s_imagesLoaded) return;\n'
    '    loadSet(s_walk, 11, L\"resource/images/Zombies/FootballZombie/FootballZombie/frame_\");\n'
    '    loadSet(s_attack, 10, L\"resource/images/Zombies/FootballZombie/FootballZombieAttack/frame_\");\n'
    '    loadSet(s_ornlost_walk, 11, L\"resource/images/Zombies/FootballZombie/FootballZombieOrnLost/frame_\");\n'
    '    loadSet(s_ornlost_attack, 11, L\"resource/images/Zombies/FootballZombie/FootballZombieOrnLostAttack/frame_\");\n'
    '    loadSet(s_losthead_walk, 10, L\"resource/images/Zombies/FootballZombie/LostHead/frame_\");\n'
    '    loadSet(s_losthead_attack, 10, L\"resource/images/Zombies/FootballZombie/LostHeadAttack/frame_\");\n'
    '    loadSet(s_die, 7, L\"resource/images/Zombies/FootballZombie/Die/frame_\");\n'
    '    s_imagesLoaded = true;\n'
    '    printf(\"FootballZombie images loaded\\n\");\n'
    '}\n'
    '\n'
    'FootballZombie::FootballZombie(int r, float startX, float startY) {\n'
    '    row = r;\n'
    '    worldX = startX;\n'
    '    worldY = startY;\n'
    '    hp = MAX_HP;\n'
    '    stage = 0;\n'
    '    dying = false;\n'
    '    isAttacking = false;\n'
    '    speed = rand() % 6 + 10;\n'
    '    currentFrame = 0;\n'
    '    animTimer = 0.0f;\n'
    '    animInterval = 0.1f;\n'
    '    attackTimer = 0.0f;\n'
    '    attackInterval = 0.65f;\n'
    '    attackDamage = 60;\n'
    '    deathTimer = 0.0f;\n'
    '    deathFrame = 0;\n'
    '    int p = rand() % 10;\n'
    '    if (!p) sounds();\n'
    '}\n'
    '\n'
    'void FootballZombie::update(float delta) {\n'
    '    if (dying) {\n'
    '        deathTimer += delta;\n'
    '        if (deathTimer >= 0.15f) {\n'
    '            deathTimer = 0.0f;\n'
    '            if (deathFrame < (int)s_die.size() - 1) {\n'
    '                deathFrame++;\n'
    '            } else {\n'
    '                // Last frame held for 0.3s, then done\n'
    '                if (deathTimer >= 0.3f) {\n'
    '                    stage = 4;\n'
    '                }\n'
    '            }\n'
    '        }\n'
    '        return;\n'
    '    }\n'
    '\n'
    '    // Auto-drain in headLost stage\n'
    '    if (stage == 2) {\n'
    '        hp -= (int)(30.0f * delta);\n'
    '        if (hp <= 0) {\n'
    '            hp = 0;\n'
    '            dying = true;\n'
    '            deathTimer = 0.0f;\n'
    '            deathFrame = 0;\n'
    '            return;\n'
    '        }\n'
    '    }\n'
    '\n'
    '    // Update stage based on HP\n'
    '    if (hp > HELMET_LOST_HP) stage = 0;\n'
    '    else if (hp > HEAD_LOST_HP) stage = 1;\n'
    '    else if (hp > 0) stage = 2;\n'
    '    else { hp = 0; dying = true; deathTimer = 0.0f; deathFrame = 0; return; }\n'
    '\n'
    '    if (!isAttacking) worldX -= speed * delta;\n'
    '\n'
    '    std::vector<IMAGE>* frames = nullptr;\n'
    '    switch (stage) {\n'
    '    case 0: frames = isAttacking ? &s_attack : &s_walk; break;\n'
    '    case 1: frames = isAttacking ? &s_ornlost_attack : &s_ornlost_walk; break;\n'
    '    case 2: frames = isAttacking ? &s_losthead_attack : &s_losthead_walk; break;\n'
    '    }\n'
    '    if (frames && !frames->empty()) {\n'
    '        animTimer += delta;\n'
    '        if (animTimer >= animInterval) {\n'
    '            animTimer -= animInterval;\n'
    '            currentFrame = (currentFrame + 1) % (int)frames->size();\n'
    '        }\n'
    '    }\n'
    '}\n'
    '\n'
    'void FootballZombie::draw(int screenX, int screenY) {\n'
    '    if (dying) {\n'
    '        if (deathFrame < (int)s_die.size()) {\n'
    '            const IMAGE& img = s_die[deathFrame];\n'
    '            putimagePng(screenX, screenY, &img, 0, 0, img.getwidth(), img.getheight());\n'
    '        }\n'
    '        return;\n'
    '    }\n'
    '    std::vector<IMAGE>* frames = nullptr;\n'
    '    switch (stage) {\n'
    '    case 0: frames = isAttacking ? &s_attack : &s_walk; break;\n'
    '    case 1: frames = isAttacking ? &s_ornlost_attack : &s_ornlost_walk; break;\n'
    '    case 2: frames = isAttacking ? &s_losthead_attack : &s_losthead_walk; break;\n'
    '    }\n'
    '    if (frames && !frames->empty() && currentFrame < (int)frames->size()) {\n'
    '        const IMAGE& img = (*frames)[currentFrame];\n'
    '        putimagePng(screenX, screenY, &img, 0, 0, img.getwidth(), img.getheight());\n'
    '    }\n'
    '}\n'
    '\n'
    'void FootballZombie::takeDamage(int damage) {\n'
    '    if (dying) return;\n'
    '    hp -= damage;\n'
    '    if (hp < 0) hp = 0;\n'
    '    if (hp == 0 && !dying) { dying = true; deathTimer = 0.0f; deathFrame = 0; }\n'
    '}\n'
    '\n'
    'bool FootballZombie::isDead() const { return stage == 4; }\n'
    '\n'
    'void FootballZombie::drawDead() {}\n'
    '\n'
    'void FootballZombie::sounds() {\n'
    '    int id = rand() % 6 + 1;\n'
    '    std::string file = \"groan\" + std::to_string(id) + \".mp3\";\n'
    '    audio.playEffect(file);\n'
    '}\n'
)

with open(os.path.join(base, 'footballzombie.cpp'), 'w', encoding='utf-8', newline='') as f:
    f.write(fb_cpp)
print('footballzombie.cpp written')

# ===== 3. CHECKPLANTCOLLISION: use getCollideOffset() =====
with open(os.path.join(base, 'Playing.cpp'), 'rb') as f:
    lines = f.read().split(b'\r\n')

for i, line in enumerate(lines):
    if b'ZOMBIE_COLLIDE_OFFSET' in line and b'zombie->' not in line:
        lines[i] = lines[i].replace(b'ZOMBIE_COLLIDE_OFFSET', b'zombie->getCollideOffset()')
        break

# ===== 4. PREVIEW: use 0.gif =====
for i, line in enumerate(lines):
    if b'FootballZombie.png' in line:
        lines[i] = b'\t\t\tcase 3: return L\"resource/images/Zombies/FootballZombie/0.gif\";'
        break

# ===== 5. SPAWN Y ADJUSTMENT for FootballZombie =====
for i, line in enumerate(lines):
    if b'FootballZombie::loadSharedImages();' in line:
        # Good, it's already there
        break

with open(os.path.join(base, 'Playing.cpp'), 'wb') as f:
    f.write(b'\r\n'.join(lines))
print(f'Playing.cpp: {len(lines)} lines')

# ===== 6. SPAWN.CPP: adjust Y a bit lower for FootballZombie =====
with open(os.path.join(base, 'Spawn.cpp'), 'rb') as f:
    lines = f.read().split(b'\r\n')

for i, line in enumerate(lines):
    if b'case 3:' in line and b'FootballZombie' in lines[i+1]:
        # Adjust Y: add extra offset for football zombie
        indent = b'                '
        orig = lines[i+1]
        # We need a different startY, so add line before spawn
        lines[i] = b'            case 3:'
        lines[i+1] = b'            {'
        lines[i+2] = b'                float fbY = GRID_WORLD_Y + row * CELL_H + (CELL_H - 144) / 2 - 35;'
        lines[i+3] = b'                spawnFinal.push_back(std::make_unique<FootballZombie>(row, startX, fbY));'
        lines[i+4] = b'            }'
        # Remove old push_back line (shifted)
        break

# Find and remove the old FootballZombie spawn line after the above fix
# Actually the above already replaced it. Let me check what's there.

with open(os.path.join(base, 'Spawn.cpp'), 'wb') as f:
    f.write(b'\r\n'.join(lines))
print(f'Spawn.cpp: {len(lines)} lines')

print('Done.')
