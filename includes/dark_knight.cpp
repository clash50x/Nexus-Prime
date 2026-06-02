// ============================================================
//  THE FALLEN KINGDOM
//  2D Side-Scrolling Action RPG
//  C++17 | SFML 3.1.0 | Procedural Only (no OOP)
// ============================================================

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cstdint>

// ============================================================
//  CONSTANTS
// ============================================================
const int   WINDOW_W        = 1280;
const int   WINDOW_H        = 720;
const float GRAVITY         = 1800.f;
const float GROUND_Y        = 580.f;         // y where feet rest
const float SPRITE_SCALE    = 3.2f;    // fallback for princess etc.

// Desired on-screen CHARACTER heights (pixels). scale = TARGET / charH keeps
// every sprite the same visual size across idle/run/attack sheets.
const float HERO_TARGET_H   = 150.f;
const float GOBLIN_TARGET_H = 110.f;
const float KNIGHT_TARGET_H = 130.f;
const float CMD_TARGET_H    = 145.f;
const float BOSS_TARGET_H   = 320.f;
const int   FRAME_W         = 62;
const int   FRAME_H         = 62;
const int   SHEET_COLS      = 11;

// Per-sheet layout info — cols/rows define the grid, charH is the actual
// character pixel height measured inside one frame (excludes black padding).
// scale = TARGET_H / charH keeps the on-screen character the same size
// regardless of how much padding the artist left around each frame.
struct SheetInfo {
    int cols;
    int rows;
    int charH;   // measured character height in pixels within one frame
};

// charH values measured via pixel bounding-box on the actual sprite sheets.
// All sheets share the same 677x369 canvas; charH is the artist's character
// height inside that canvas (or inside the sub-frame for multi-row sheets).

// Hero — sheets not uploaded; using same 677x369 canvas convention
const SheetInfo HERO_IDLE_INFO   = { 1, 1, 334 };   // full canvas, charH ~334
const SheetInfo HERO_RUN_INFO    = { 5, 2, 155 };   // 184px tall frames, charH ~155
const SheetInfo HERO_ATTACK_INFO = { 3, 2, 155 };   // 184px tall frames, charH ~155

// enemy1 (goblin): idle=1×1, run=3×2, attack=4×2   (all 677×369 canvas)
const SheetInfo GOBLIN_IDLE_INFO   = { 1, 1, 330 };  // full canvas
const SheetInfo GOBLIN_RUN_INFO    = { 3, 2, 155 };  // 184px tall sub-frames
const SheetInfo GOBLIN_ATTACK_INFO = { 4, 2, 155 };  // 184px tall sub-frames

// enemy_2 (dark knight): idle=1×1, run=5×1, attack=5×1  — measured from uploaded images
// All sheets are 677x369; run/attack are single-row so frameH=369 but char sits in middle
const SheetInfo KNIGHT_IDLE_INFO   = { 1, 1, 164 };  // corrected: same as run for consistency
const SheetInfo KNIGHT_RUN_INFO    = { 5, 1, 164 };  // measured avg: 164px in 369px frame
const SheetInfo KNIGHT_ATTACK_INFO = { 5, 1, 150 };  // measured avg: 150px in 369px frame

// special_enemy (commander): idle=1×1, run=5×2, attack=5×1  — measured from uploaded images
const SheetInfo CMD_IDLE_INFO   = { 1, 1, 334 };  // measured: 334px
const SheetInfo CMD_RUN_INFO    = { 5, 2, 158 };  // measured avg: 158px in 184px frame
const SheetInfo CMD_ATTACK_INFO = { 5, 1, 176 };  // measured avg: 176px in 369px frame

// boss: idle=1×1, run=4×2, attack=4×2  — measured from uploaded images
const SheetInfo BOSS_IDLE_INFO   = { 1, 1, 342 };  // measured: 342px
const SheetInfo BOSS_RUN_INFO    = { 4, 2, 127 };  // measured: 127px in 184px frame
const SheetInfo BOSS_ATTACK_INFO = { 4, 2, 157 };  // measured: 157px in 184px frame

const float WORLD_WIDTH     = 6000.f;

// ============================================================
//  ENUMS
// ============================================================
enum GameState { STATE_SPLASH, STATE_MENU, STATE_STORY, STATE_PLAYING, STATE_PAUSE, STATE_VICTORY, STATE_DEFEAT };
enum AnimState { ANIM_IDLE, ANIM_RUN, ANIM_ATTACK, ANIM_HURT, ANIM_DEATH };
enum EnemyType { ENEMY_GOBLIN, ENEMY_DARKKNIGHT, ENEMY_COMMANDER };
enum PickupType { PICKUP_HEALTH, PICKUP_COIN };

// ============================================================
//  STRUCTS
// ============================================================
struct Animation {
    int   startRow   = 0;
    int   startCol   = 0;
    int   frameCount = 6;
    float frameTime  = 0.12f;
    bool  looping    = true;
    float timer      = 0.f;
    int   current    = 0;
    bool  finished   = false;
};

struct Player {
    sf::Vector2f pos    = { 100.f, GROUND_Y };
    sf::Vector2f vel    = { 0.f,   0.f };
    float width  = 60.f;
    float height = 150.f;  
    int   hp            = 350;
    int   maxHp         = 350;
    int   damage        = 35;
    float speed         = 320.f;
    float attackCooldown = 0.f;
    float attackDuration = 0.f;
    float hurtTimer     = 0.f;
    float invincTimer   = 0.f;   // brief iframes after hit
    float deathTimer    = 0.f;
    bool  onGround      = true;
    bool  facingRight   = true;
    bool  isAttacking   = false;
    bool  attackSoundPlayed = false;  // prevent slash sound from playing multiple times per attack
    bool  isDead        = false;
    AnimState animState = ANIM_IDLE;
    Animation anim;
    // hit box for sword (extends in front)
    sf::FloatRect attackBox = sf::FloatRect({0.f, 0.f}, {0.f, 0.f});
};

struct Enemy {
    sf::Vector2f pos;
    sf::Vector2f vel     = { 0.f, 0.f };
    float width          = 80.f;
    float height         = 128.f;
    int   hp;
    int   maxHp;
    int   damage;
    float speed;
    float attackCooldown = 0.f;
    float attackDuration = 0.f;
    float hurtTimer      = 0.f;
    float aggroRange     = 400.f;
    float attackRange    = 90.f;
    bool  onGround       = true;
    bool  facingRight    = false;
    bool  isAttacking    = false;
    bool  isDead         = false;
    bool  active         = true;
    float deathTimer     = 0.f;
    EnemyType type;
    AnimState animState  = ANIM_IDLE;
    Animation anim;
    // patrol
    float patrolLeft     = 0.f;
    float patrolRight    = 0.f;
    int   patrolDir      = 1;
    // commander dash
    float dashTimer      = 0.f;
    bool  dashing        = false;
};

struct Boss {
    sf::Vector2f pos     = { 5200.f, GROUND_Y };
    sf::Vector2f vel     = { 0.f, 0.f };
    float width  = 220.f;
    float height = 320.f;
    int   hp             = 1200;
    int   maxHp          = 1200;
    int   damage         = 55;
    float speed          = 110.f;
    float attackCooldown = 0.f;
    float attackDuration = 0.f;
    float hurtTimer      = 0.f;
    bool  facingRight    = false;
    bool  isAttacking    = false;
    bool  isDead         = false;
    bool  active         = false;     // activated when player is close
    float deathTimer     = 0.f;
    int   phase          = 1;
    float chargeTimer    = 0.f;
    bool  charging       = false;
    float rageFlash      = 0.f;
    AnimState animState  = ANIM_IDLE;
    Animation anim;
};

struct Pickup {
    sf::Vector2f pos;
    PickupType type;
    bool active = true;
    float bobTimer = 0.f;
};

struct DamageText {
    sf::Vector2f pos;
    float velY       = -120.f;
    float alpha      = 255.f;
    float lifetime   = 1.0f;
    int   value;
    bool  active     = true;
    sf::Color color  = sf::Color::Yellow;
};

struct Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::Color    color;
    float        lifetime;
    float        maxLife;
    bool         active = true;
};

struct Level {
    int   id;
    std::string name;
    std::string bgFile;
    float worldWidth;
    bool  hasBoss;
    bool  hasMiniBoss;
    std::vector<Enemy> enemies;
};

// ============================================================
//  GLOBAL GAME STATE
// ============================================================
GameState  gState        = STATE_SPLASH;
int        gCurrentLevel = 0;
int        gScore        = 0;
int        gKills        = 0;
int        gCoins        = 0;
float      gShakeTimer   = 0.f;
float      gShakeAmount  = 0.f;
float      gSplashTimer  = 0.f;
float      gStoryPage    = 0.f;

Player     gPlayer;
Boss       gBoss;
std::vector<Enemy>      gEnemies;
std::vector<Pickup>     gPickups;
std::vector<DamageText> gDamageTexts;
std::vector<Particle>   gParticles;

// Textures (one per sprite sheet / image)
sf::Texture texHeroIdle, texHeroRun, texHeroAttack;
sf::Texture texGoblinIdle, texGoblinRun, texGoblinAttack;
sf::Texture texKnightIdle, texKnightRun, texKnightAttack;
sf::Texture texCmdIdle, texCmdRun, texCmdAttack;
sf::Texture texBossIdle, texBossRun, texBossAttack;
sf::Texture texPrincess;
sf::Texture texBg1, texBg2, texBg3;
sf::Texture texWin, texLoss;

// Princess
sf::Vector2f gPrincessPos = { 5500.f, GROUND_Y };
bool         gPrincessVisible = false;

// Camera
float gCamX = 0.f;
float gCamTargetX = 0.f;

// Font
sf::Font gFont;

// Audio

sf::SoundBuffer bufferSlash;
sf::Music soundBgm;
std::optional<sf::Sound> soundSlash;
bool bgmPlaying = false;

// Menu cursor
int gMenuCursor = 0;

// Story text
const char* STORY_LINES[] = {
    "Long ago, the Kingdom of Eldoria",
    "was protected by the Royal Guardian.",
    "",
    "One night, the Dark Emperor invaded.",
    "His armies slaughtered the royal soldiers.",
    "The Princess was captured and imprisoned.",
    "",
    "The kingdom collapsed into darkness.",
    "",
    "You are the last surviving warrior.",
    "Travel through dangerous lands,",
    "defeat the Dark Emperor,",
    "and rescue the Princess.",
    "",
    "The kingdom depends on you.",
    "",
    "[ Press ENTER to begin ]"
};
const int STORY_LINE_COUNT = 16;

// Level configs
struct LevelConfig {
    const char* name;
    const char* bgKey;  // "bg1", "bg2", "bg3"
    int goblins;
    int knights;
    int commanders;
    bool miniBoss;
    bool boss;
    float width;
};

LevelConfig LEVEL_CONFIGS[4] = {
    { "LEVEL 1 - Forgotten Forest", "bg1", 8,  0, 0, false, false, 3000.f },
    { "LEVEL 2 - Dark Valley",      "bg2", 5,  5, 0, false, false, 3500.f },
    { "LEVEL 3 - Cursed Battlefield","bg1", 4,  6, 3, true,  false, 4000.f },
    { "LEVEL 4 - Dark Fortress",    "bg3", 0,  4, 2, false, true,  6000.f },
};

// ============================================================
//  UTILITY FUNCTIONS
// ============================================================
float lerp(float a, float b, float t) { return a + (b - a) * t; }

float distance(sf::Vector2f a, sf::Vector2f b) {
    float dx = b.x - a.x, dy = b.y - a.y;
    return std::sqrt(dx*dx + dy*dy);
}

sf::FloatRect makeRect(sf::Vector2f pos, float w, float h) {
    return sf::FloatRect({ pos.x - w * 0.5f, pos.y - h }, { w, h });
}

bool rectsOverlap(sf::FloatRect a, sf::FloatRect b) {
    return a.findIntersection(b).has_value();
}

std::string intToStr(int v) {
    std::ostringstream ss; ss << v; return ss.str();
}

void spawnShake(float amount, float duration) {
    gShakeAmount = amount;
    gShakeTimer  = duration;
}

void spawnDamageText(sf::Vector2f pos, int value, sf::Color col = sf::Color::Yellow) {
    DamageText dt;
    dt.pos   = pos;
    dt.value = value;
    dt.color = col;
    gDamageTexts.push_back(dt);
}

void spawnParticles(sf::Vector2f pos, sf::Color col, int count = 8) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.pos = pos;
        float angle = (rand() % 360) * 3.14159f / 180.f;
        float spd   = 80.f + rand() % 200;
        p.vel   = { std::cos(angle) * spd, std::sin(angle) * spd - 100.f };
        p.color = col;
        p.lifetime = p.maxLife = 0.4f + (rand() % 40) * 0.01f;
        gParticles.push_back(p);
    }
}

void spawnPickup(sf::Vector2f pos, PickupType type) {
    Pickup pk;
    pk.pos  = pos;
    pk.type = type;
    gPickups.push_back(pk);
}

// ============================================================
//  ANIMATION FUNCTIONS
// ============================================================
void initAnim(Animation& a, int row, int col, int frames, float frameTime, bool looping) {
    a.startRow   = row;
    a.startCol   = col;
    a.frameCount = frames;
    a.frameTime  = frameTime;
    a.looping    = looping;
    a.timer      = 0.f;
    a.current    = 0;
    a.finished   = false;
}

void updateAnim(Animation& a, float dt) {
    if (a.finished && !a.looping) return;
    a.timer += dt;
    if (a.timer >= a.frameTime) {
        a.timer -= a.frameTime;
        a.current++;
        if (a.current >= a.frameCount) {
            if (a.looping) a.current = 0;
            else { a.current = a.frameCount - 1; a.finished = true; }
        }
    }
}

sf::IntRect getAnimRect(const Animation& a, const sf::Texture& tex,
                        int sheetCols = 11, int sheetRows = 6)
{
    int frameW = static_cast<int>(tex.getSize().x / sheetCols);
    int frameH = static_cast<int>(tex.getSize().y / sheetRows);

    int col = (a.startCol + a.current) % sheetCols;
    int row = a.startRow + (a.startCol + a.current) / sheetCols;

    return sf::IntRect(
        { col * frameW, row * frameH },
        { frameW, frameH }
    );
}

void drawSprite(
    sf::RenderWindow& win,
    sf::Texture& tex,
    const Animation& anim,
    sf::Vector2f pos,
    bool facingRight,
    float scale = SPRITE_SCALE,
    sf::Color tint = sf::Color::White,
    int sheetCols = 11,
    int sheetRows = 6)
{
    sf::Sprite spr(tex);

    int frameW = static_cast<int>(tex.getSize().x / sheetCols);
    int frameH = static_cast<int>(tex.getSize().y / sheetRows);

    spr.setTextureRect(getAnimRect(anim, tex, sheetCols, sheetRows));

    spr.setScale({
        facingRight ? scale : -scale,
        scale
    });

    float sw = frameW * scale;
    float sh = frameH * scale;

    if (facingRight)
        spr.setPosition({ pos.x - sw * 0.5f, pos.y - sh });
    else
        spr.setPosition({ pos.x + sw * 0.5f, pos.y - sh });

    spr.setColor(tint);

    win.draw(spr);
}

// ============================================================
//  PLAYER ANIM HELPERS
// ============================================================
// Sprite sheet rows (each file has 6 rows of 11 = 66 frames)
// We'll treat each animation as: row 0 = frames 0-10, row 1 = 11-21, etc.
// Idle: row 0 (6 frames), Run: row 0 (8 frames), Attack: row 0 (6 frames)
// Since each sprite FILE is its own animation, we just use frames 0-N from row 0

void setPlayerAnim(Player& p, AnimState state) {
    if (p.animState == state && state != ANIM_ATTACK) return;
    p.animState = state;
    switch (state) {
        // hero.png      : 1 col x 1 row  -> 1 frame (idle pose)
        case ANIM_IDLE:   initAnim(p.anim, 0, 0, 1,  0.15f, true);  break;
        // hero_run.png  : 5 cols x 2 rows -> 10 frames total
        case ANIM_RUN:    initAnim(p.anim, 0, 0, 10, 0.08f, true);  break;
        // hero_attack.png: 3 cols x 2 rows -> 6 frames total
        case ANIM_ATTACK: initAnim(p.anim, 0, 0, 6,  0.07f, false); break;
        case ANIM_HURT:   initAnim(p.anim, 0, 0, 1,  0.10f, false); break;
        case ANIM_DEATH:  initAnim(p.anim, 0, 0, 1,  0.12f, false); break;
        default: break;
    }
}

void setEnemyAnim(Enemy& e, AnimState state) {
    // HURT always interrupts animation but does NOT cancel isAttacking —
    // let the attack timer run out naturally so enemies keep fighting while hurt.
    if (state == ANIM_HURT) {
        e.animState = ANIM_HURT;
        initAnim(e.anim, 0, 0, 1, 0.10f, false);
        return;
    }
    if (state == ANIM_DEATH) {
        e.animState = ANIM_DEATH;
        initAnim(e.anim, 0, 0, 1, 0.12f, false);
        return;
    }
    // For all other states: skip if already in that state (except ATTACK which always restarts)
    if (e.animState == state && state != ANIM_ATTACK) return;
    e.animState = state;
    switch (e.type) {
        case ENEMY_GOBLIN:
            // idle=1×1=1f, run=3×2=6f, attack=4×2=8f
            switch (state) {
                case ANIM_IDLE:   initAnim(e.anim, 0, 0, 1, 0.15f, true);  break;
                case ANIM_RUN:    initAnim(e.anim, 0, 0, 6, 0.10f, true);  break;
                case ANIM_ATTACK: initAnim(e.anim, 0, 0, 8, 0.08f, false); break;
                default: break;
            }
            break;
        case ENEMY_DARKKNIGHT:
            // idle=1×1=1f, run=5×1=5f, attack=5×1=5f
            switch (state) {
                case ANIM_IDLE:   initAnim(e.anim, 0, 0, 1, 0.15f, true);  break;
                case ANIM_RUN:    initAnim(e.anim, 0, 0, 5, 0.10f, true);  break;
                case ANIM_ATTACK: initAnim(e.anim, 0, 0, 5, 0.08f, false); break;
                default: break;
            }
            break;
        case ENEMY_COMMANDER:
            // idle=1×1=1f, run=5×2=10f, attack=5×1=5f
            switch (state) {
                case ANIM_IDLE:   initAnim(e.anim, 0, 0,  1, 0.15f, true);  break;
                case ANIM_RUN:    initAnim(e.anim, 0, 0, 10, 0.08f, true);  break;
                case ANIM_ATTACK: initAnim(e.anim, 0, 0,  5, 0.07f, false); break;
                default: break;
            }
            break;
    }
}

void setBossAnim(Boss& b, AnimState state) {
    // HURT always interrupts animation but does NOT cancel isAttacking —
    // let the attack timer run out naturally so the boss keeps fighting while hurt.
    if (state == ANIM_HURT) {
        b.animState = ANIM_HURT;
        initAnim(b.anim, 0, 0, 1, 0.10f, false);
        return;
    }
    if (state == ANIM_DEATH) {
        b.animState = ANIM_DEATH;
        initAnim(b.anim, 0, 0, 1, 0.12f, false);
        return;
    }
    if (b.animState == state && state != ANIM_ATTACK) return;
    b.animState = state;
    float spd = (b.phase == 3) ? 0.06f : (b.phase == 2) ? 0.075f : 0.09f;
    switch (state) {
        // boss: idle=1×1=1f, run=4×2=8f, attack=4×2=8f
        case ANIM_IDLE:   initAnim(b.anim, 0, 0, 1, 0.15f, true);  break;
        case ANIM_RUN:    initAnim(b.anim, 0, 0, 8, 0.09f, true);  break;
        case ANIM_ATTACK: initAnim(b.anim, 0, 0, 8, spd,   false); break;
        default: break;
    }
}

// ============================================================
//  ENEMY TEXTURE SELECTORS
// ============================================================
sf::Texture& getEnemyTexIdle(EnemyType t) {
    if (t == ENEMY_GOBLIN)     return texGoblinIdle;
    if (t == ENEMY_DARKKNIGHT) return texKnightIdle;
    return texCmdIdle;
}
sf::Texture& getEnemyTexRun(EnemyType t) {
    if (t == ENEMY_GOBLIN)     return texGoblinRun;
    if (t == ENEMY_DARKKNIGHT) return texKnightRun;
    return texCmdRun;
}
sf::Texture& getEnemyTexAttack(EnemyType t) {
    if (t == ENEMY_GOBLIN)     return texGoblinAttack;
    if (t == ENEMY_DARKKNIGHT) return texKnightAttack;
    return texCmdAttack;
}

// ============================================================
//  ASSET LOADING
// ============================================================
bool loadAssets() {
    struct { sf::Texture* tex; const char* path; } list[] = {
        { &texHeroIdle,      "GUI/assets/hero.png"                  },
        { &texHeroRun,       "GUI/assets/hero_run.png"              },
        { &texHeroAttack,    "GUI/assets/hero_attack.png"           },
        { &texGoblinIdle,    "GUI/assets/enemy1.png"                },
        { &texGoblinRun,     "GUI/assets/enemy1_run.png"            },
        { &texGoblinAttack,  "GUI/assets/enemy1_attack.png"         },
        { &texKnightIdle,    "GUI/assets/enemy_2.png"               },
        { &texKnightRun,     "GUI/assets/enemy_2_run.png"           },
        { &texKnightAttack,  "GUI/assets/enemy_2_attack.png"        },
        { &texCmdIdle,       "GUI/assets/special_enemy.png"         },
        { &texCmdRun,        "GUI/assets/special_enemy_run.png"     },
        { &texCmdAttack,     "GUI/assets/special_enemy_attack.png"  },
        { &texBossIdle,      "GUI/assets/boss.png"                  },
        { &texBossRun,       "GUI/assets/boss_run.png"              },
        { &texBossAttack,    "GUI/assets/boss_attack.png"           },
        { &texPrincess,      "GUI/assets/princess.png"              },
        { &texBg1,           "GUI/assets/background_2.png"          },
        { &texBg2,           "GUI/assets/light_background.png"      },
        { &texBg3,           "GUI/assets/background_last.png"       },
        { &texWin,           "GUI/assets/win.png"                   },
        { &texLoss,          "GUI/assets/loss.png"                  },
    };
    for (auto& e : list) {
        if (!e.tex->loadFromFile(e.path)) {
            // Create a placeholder coloured texture so game still runs
            sf::Image img({ 128u, 128u }, sf::Color(200, 50, 200, 255));
            e.tex->loadFromImage(img);
        }
        e.tex->setSmooth(false);
    }
    if (!gFont.openFromFile("data/fonts/font.TTF")) {
        // fallback: no font (text won't render but game runs)
    }
    
    // Load audio files
    if (soundBgm.openFromFile("data/audio/background.wav")) {
        soundBgm.setLooping(true);
        soundBgm.setVolume(50.f);  // 50% volume for background music
    } else {
    }
    if (bufferSlash.loadFromFile("data/audio/slash.wav")) {
        soundSlash.emplace(bufferSlash);
        soundSlash->setVolume(100.f);  // Full volume for attack sound
    }
    return true;
}

// ============================================================
//  LEVEL SETUP
// ============================================================
Enemy makeEnemy(EnemyType type, float x) {
    Enemy e;
    e.type = type;
    e.pos  = { x, GROUND_Y };
    e.patrolLeft  = x - 200.f;
    e.patrolRight = x + 200.f;
    switch (type) {
        case ENEMY_GOBLIN:
            e.hp = e.maxHp = 80;
            e.damage = 15;
            e.speed  = 130.f;
            e.aggroRange = 350.f;
            break;
        case ENEMY_DARKKNIGHT:
            e.hp = e.maxHp = 150;
            e.damage = 28;
            e.speed  = 170.f;
            e.aggroRange = 400.f;
            break;
        case ENEMY_COMMANDER:
            e.hp = e.maxHp = 280;
            e.damage = 40;
            e.speed  = 150.f;
            e.aggroRange = 500.f;
            e.attackRange = 110.f;
            break;
    }
    setEnemyAnim(e, ANIM_IDLE);
    return e;
}

// "Mini boss" is just a commander with boosted stats
Enemy makeMiniBoss(float x) {
    Enemy e = makeEnemy(ENEMY_COMMANDER, x);
    e.hp = e.maxHp = 600;
    e.damage = 50;
    e.speed  = 160.f;
    return e;
}

void buildLevel(int id) {
    gEnemies.clear();
    gPickups.clear();
    gDamageTexts.clear();
    gParticles.clear();
    gPrincessVisible = false;

    LevelConfig& cfg = LEVEL_CONFIGS[id];
    float ww = cfg.width;

    // Scatter enemies across the level width
    int total = cfg.goblins + cfg.knights + cfg.commanders;
    float spacing = (ww - 400.f) / std::max(total, 1);
    float xOff = 400.f;

    for (int i = 0; i < cfg.goblins; i++) {
        gEnemies.push_back(makeEnemy(ENEMY_GOBLIN, xOff));
        xOff += spacing;
    }
    for (int i = 0; i < cfg.knights; i++) {
        gEnemies.push_back(makeEnemy(ENEMY_DARKKNIGHT, xOff));
        xOff += spacing;
    }
    for (int i = 0; i < cfg.commanders; i++) {
        gEnemies.push_back(makeEnemy(ENEMY_COMMANDER, xOff));
        xOff += spacing;
    }
    if (cfg.miniBoss) {
        gEnemies.push_back(makeMiniBoss(ww - 400.f));
    }

    // Boss
    gBoss = Boss{};
    gBoss.pos = { ww - 500.f, GROUND_Y };
    setBossAnim(gBoss, ANIM_IDLE);
    gBoss.active = cfg.boss;

    // Place some health pickups mid-level
    for (int i = 1; i <= 3; i++) {
        spawnPickup({ ww * 0.25f * i, GROUND_Y - 20.f }, PICKUP_HEALTH);
    }

    // Reset player position, not stats
    gPlayer.pos = { 150.f, GROUND_Y };
    gPlayer.vel = { 0.f, 0.f };
    gPlayer.isAttacking = false;
    gPlayer.hurtTimer   = 0.f;
    gPlayer.invincTimer = 0.f;
    gPlayer.attackCooldown = 0.f;
    if (gPlayer.isDead) {
        gPlayer.isDead = false;
        gPlayer.hp     = gPlayer.maxHp;
    }
    setPlayerAnim(gPlayer, ANIM_IDLE);

    gCamX = 0.f;
    gCamTargetX = 0.f;
}

void startGame() {
    gCurrentLevel = 0;
    gScore = 0; gKills = 0; gCoins = 0;
    gPlayer.hp    = gPlayer.maxHp = 350;
    gPlayer.damage = 35;
    gPlayer.isDead = false;
    gPlayer.pos    = { 150.f, GROUND_Y };
    setPlayerAnim(gPlayer, ANIM_IDLE);
    buildLevel(0);
    gState = STATE_PLAYING;
    
    // Start background music
    if (soundBgm.getDuration().asSeconds() > 0.f) {
        soundBgm.play();
        bgmPlaying = true;
    }
}

// ============================================================
//  PLAYER UPDATE
// ============================================================
void updatePlayer(float dt) {
    Player& p = gPlayer;
    if (p.isDead) {
        updateAnim(p.anim, dt);
        p.deathTimer += dt;
        if (p.deathTimer > 2.5f) gState = STATE_DEFEAT;
        return;
    }

    // Timers
    if (p.attackCooldown > 0.f) p.attackCooldown -= dt;
    if (p.hurtTimer > 0.f)      p.hurtTimer      -= dt;
    if (p.invincTimer > 0.f)    p.invincTimer    -= dt;

    // Input
    bool left  = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
    bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
    bool jump  = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    // Horizontal movement
    if (!p.isAttacking) {
        if (left)  { p.vel.x = -p.speed; p.facingRight = false; }
        else if (right) { p.vel.x = p.speed; p.facingRight = true; }
        else p.vel.x = 0.f;
    } else {
        p.vel.x *= 0.6f;  // slow during attack
    }

    // Jump
    static bool jumpHeld = false;
    if (jump && p.onGround && !jumpHeld) {
        p.vel.y = -700.f;
        p.onGround = false;
    }
    jumpHeld = jump;

    // Gravity
    if (!p.onGround) p.vel.y += GRAVITY * dt;

    // Position
    p.pos.x += p.vel.x * dt;
    p.pos.y += p.vel.y * dt;

    // Ground clamp
    float curWorldW = LEVEL_CONFIGS[gCurrentLevel].width;
    if (p.pos.y >= GROUND_Y) {
        p.pos.y   = GROUND_Y;
        p.vel.y   = 0.f;
        p.onGround = true;
    }
    p.pos.x = std::max(50.f, std::min(p.pos.x, curWorldW - 50.f));

    // Attack input (Z or J or LCtrl)
    bool attackKey = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)    ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J)    ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl);
    if (attackKey && p.attackCooldown <= 0.f && !p.isAttacking) {
        p.isAttacking    = true;
        p.attackDuration = 0.36f;
        p.attackCooldown = 0.45f;
        p.attackSoundPlayed = false;  // Reset sound flag for new attack
        setPlayerAnim(p, ANIM_ATTACK);
    }

    if (p.isAttacking) {
        p.attackDuration -= dt;
        // Build attack hitbox — hero is ~150px tall on screen, chest ~90px up
        float reach = 100.f;
        if (p.facingRight)
            p.attackBox = sf::FloatRect({ p.pos.x + 15.f,         p.pos.y - 110.f }, { reach, 70.f });
        else
            p.attackBox = sf::FloatRect({ p.pos.x - 15.f - reach, p.pos.y - 110.f }, { reach, 70.f });

        if (p.attackDuration <= 0.f) {
            p.isAttacking = false;
            p.attackSoundPlayed = false;  // Reset flag when attack ends
            p.attackBox   = sf::FloatRect({0.f, 0.f}, {0.f, 0.f});
        }
    } else {
        p.attackBox = sf::FloatRect({0.f, 0.f}, {0.f, 0.f});
    }

    // Animation state machine
    if (p.hurtTimer > 0.f && p.animState != ANIM_HURT) {
        setPlayerAnim(p, ANIM_HURT);
    } else if (!p.isAttacking && p.hurtTimer <= 0.f) {
        if (std::abs(p.vel.x) > 10.f) setPlayerAnim(p, ANIM_RUN);
        else setPlayerAnim(p, ANIM_IDLE);
    }

    updateAnim(p.anim, dt);
}

// ============================================================
//  ENEMY UPDATE
// ============================================================
void updateEnemy(Enemy& e, float dt) {
    if (!e.active) return;
    if (e.isDead) {
        updateAnim(e.anim, dt);
        e.deathTimer += dt;
        if (e.deathTimer > 1.5f) e.active = false;
        return;
    }

    if (e.attackCooldown > 0.f) e.attackCooldown -= dt;
    if (e.hurtTimer > 0.f)      e.hurtTimer      -= dt;

    float dist = distance(e.pos, gPlayer.pos);
    bool  playerDead = gPlayer.isDead;

    // Commander dash cooldown
    if (e.type == ENEMY_COMMANDER) {
        if (e.dashTimer > 0.f) e.dashTimer -= dt;
    }

    if (!playerDead && dist < e.aggroRange) {
        // Chase
        float dir = (gPlayer.pos.x > e.pos.x) ? 1.f : -1.f;
        e.facingRight = (dir > 0);

        if (e.type == ENEMY_COMMANDER && e.dashTimer <= 0.f && dist < 300.f && !e.dashing) {
            // dash attack
            e.dashing = true;
            e.dashTimer = 2.5f;
        }

        if (e.dashing) {
            e.vel.x = dir * e.speed * 2.5f;
            e.dashing = false;
        }

        if (dist > e.attackRange) {
            e.vel.x = dir * e.speed;
            setEnemyAnim(e, ANIM_RUN);
        } else {
            e.vel.x = 0.f;
            // Attack
            if (e.attackCooldown <= 0.f && !e.isAttacking) {
                e.isAttacking    = true;
                e.attackDuration = 0.4f;
                e.attackCooldown = (e.type == ENEMY_DARKKNIGHT) ? 1.2f : 1.6f;
                setEnemyAnim(e, ANIM_ATTACK);
            }
        }
    } else {
        // Patrol
        e.vel.x = e.patrolDir * (e.speed * 0.4f);
        e.facingRight = (e.patrolDir > 0);
        if (e.pos.x >= e.patrolRight) e.patrolDir = -1;
        if (e.pos.x <= e.patrolLeft)  e.patrolDir =  1;
        setEnemyAnim(e, ANIM_IDLE);
    }

    // Attack duration / hitbox check
    if (e.isAttacking) {
        e.attackDuration -= dt;
        if (e.attackDuration <= 0.f) e.isAttacking = false;

        // Deal damage at mid-attack — use a forward reach box so distance doesn't have to be zero
        if (e.attackDuration < 0.25f && e.attackDuration > 0.05f) {
            float reach = e.attackRange * 0.9f;
            sf::FloatRect eRect = e.facingRight
                ? sf::FloatRect({ e.pos.x,         e.pos.y - e.height }, { reach, e.height })
                : sf::FloatRect({ e.pos.x - reach,  e.pos.y - e.height }, { reach, e.height });
            sf::FloatRect pRect = makeRect(gPlayer.pos, gPlayer.width, gPlayer.height);
            if (rectsOverlap(eRect, pRect) && gPlayer.invincTimer <= 0.f) {
                gPlayer.hp -= e.damage;
                gPlayer.invincTimer = 0.6f;
                gPlayer.hurtTimer   = 0.3f;
                spawnDamageText(gPlayer.pos + sf::Vector2f{0,-30.f}, e.damage, sf::Color::Red);
                spawnShake(5.f, 0.15f);
                if (gPlayer.hp <= 0) {
                    gPlayer.hp   = 0;
                    gPlayer.isDead = true;
                    gPlayer.deathTimer = 0.f;
                    setPlayerAnim(gPlayer, ANIM_DEATH);
                }
            }
        }
    }

    // Gravity & movement
    if (!e.onGround) e.vel.y += GRAVITY * dt;
    e.pos.x += e.vel.x * dt;
    e.pos.y += e.vel.y * dt;
    if (e.pos.y >= GROUND_Y) { e.pos.y = GROUND_Y; e.vel.y = 0.f; e.onGround = true; }
    e.vel.x *= 0.8f;

    // Only show hurt flash when NOT actively attacking — attack animation takes priority
    if (e.hurtTimer > 0.f && !e.isAttacking && e.animState != ANIM_HURT) setEnemyAnim(e, ANIM_HURT);

    updateAnim(e.anim, dt);
}

// ============================================================
//  BOSS UPDATE
// ============================================================
void updateBoss(float dt) {
    Boss& b = gBoss;
    if (!b.active) return;

    if (b.isDead) {
        updateAnim(b.anim, dt);
        b.deathTimer += dt;
        if (b.deathTimer > 2.5f) {
            gPrincessVisible = true;
            // short delay then victory
        }
        if (b.deathTimer > 4.0f) gState = STATE_VICTORY;
        return;
    }

    if (b.attackCooldown > 0.f) b.attackCooldown -= dt;
    if (b.hurtTimer > 0.f)      b.hurtTimer      -= dt;
    if (b.rageFlash > 0.f)      b.rageFlash      -= dt;

    // Phase transitions
    float hpPct = (float)b.hp / (float)b.maxHp;
    if (hpPct <= 0.5f && b.phase < 2) {
        b.phase = 2;
        b.speed = 145.f;
        b.damage = 70;
        spawnShake(12.f, 0.4f);
        b.rageFlash = 1.0f;
    }
    if (hpPct <= 0.2f && b.phase < 3) {
        b.phase = 3;
        b.speed = 185.f;
        b.damage = 90;
        spawnShake(18.f, 0.5f);
        b.rageFlash = 1.5f;
    }

    float dist = distance(b.pos, gPlayer.pos);

    // AI
    float dir = (gPlayer.pos.x > b.pos.x) ? 1.f : -1.f;
    b.facingRight = (dir > 0);

    // Phase 2: charge
    if (b.phase >= 2 && b.chargeTimer <= 0.f && dist < 600.f && !b.charging) {
        b.charging   = true;
        b.chargeTimer = 3.0f;
    }
    if (b.chargeTimer > 0.f) b.chargeTimer -= dt;

    if (b.charging) {
        b.vel.x = dir * b.speed * 3.f;
        b.charging = false;
        spawnShake(8.f, 0.2f);
    }

    if (dist > 100.f) {
        b.vel.x = dir * b.speed;
        setBossAnim(b, ANIM_RUN);
    } else {
        b.vel.x = 0.f;
        if (b.attackCooldown <= 0.f && !b.isAttacking) {
            b.isAttacking    = true;
            b.attackDuration = 0.5f;
            float cd = (b.phase == 3) ? 1.0f : (b.phase == 2) ? 1.4f : 1.8f;
            b.attackCooldown = cd;
            setBossAnim(b, ANIM_ATTACK);
        }
    }

    if (b.isAttacking) {
        b.attackDuration -= dt;
        if (b.attackDuration < 0.35f && b.attackDuration > 0.05f) {
            float reach = 160.f;  // boss has long arms
            sf::FloatRect bRect = b.facingRight
                ? sf::FloatRect({ b.pos.x,        b.pos.y - b.height }, { reach, b.height })
                : sf::FloatRect({ b.pos.x - reach, b.pos.y - b.height }, { reach, b.height });
            sf::FloatRect pRect = makeRect(gPlayer.pos, gPlayer.width, gPlayer.height);
            if (rectsOverlap(bRect, pRect) && gPlayer.invincTimer <= 0.f) {
                gPlayer.hp -= b.damage;
                gPlayer.invincTimer = 0.7f;
                gPlayer.hurtTimer   = 0.35f;
                spawnDamageText(gPlayer.pos + sf::Vector2f{0,-30.f}, b.damage, sf::Color::Red);
                spawnShake(10.f, 0.25f);
                if (gPlayer.hp <= 0) {
                    gPlayer.hp     = 0;
                    gPlayer.isDead = true;
                    gPlayer.deathTimer = 0.f;
                    setPlayerAnim(gPlayer, ANIM_DEATH);
                }
            }
        }
        if (b.attackDuration <= 0.f) b.isAttacking = false;
    }

    b.pos.x += b.vel.x * dt;
    b.vel.x *= 0.75f;
    b.pos.x = std::max(b.pos.x, 0.f);
    b.pos.x = std::min(b.pos.x, LEVEL_CONFIGS[gCurrentLevel].width - 100.f);

    if (b.hurtTimer > 0.f && !b.isAttacking) setBossAnim(b, ANIM_HURT);
    else if (!b.isAttacking && b.attackCooldown <= 0.f && std::abs(b.vel.x) < 5.f) setBossAnim(b, ANIM_IDLE);

    updateAnim(b.anim, dt);
}

// ============================================================
//  COMBAT: PLAYER HIT ENEMIES
// ============================================================
void checkPlayerAttackHits() {
    if (!gPlayer.isAttacking) return;
    sf::FloatRect attackBox = gPlayer.attackBox;

    // Hit enemies
    for (auto& e : gEnemies) {
        if (!e.active || e.isDead) continue;
        sf::FloatRect eRect = makeRect(e.pos, e.width, e.height);
        if (rectsOverlap(attackBox, eRect)) {
            // Prevent multiple hits per swing using hurtTimer
            if (e.hurtTimer > 0.f) continue;
            e.hp        -= gPlayer.damage;
            e.hurtTimer  = 0.4f;
            e.vel.x = (e.pos.x > gPlayer.pos.x ? 1.f : -1.f) * 120.f;
            spawnDamageText(e.pos + sf::Vector2f{0,-20.f}, gPlayer.damage);
            spawnParticles(e.pos + sf::Vector2f{0,-60.f}, sf::Color::Red, 6);
            spawnShake(3.f, 0.1f);
            
            // Play slash sound only once per attack
            if (!gPlayer.attackSoundPlayed && bufferSlash.getDuration().asSeconds() > 0.f) {
                soundSlash->play();
                gPlayer.attackSoundPlayed = true;
            }
            // Don't force HURT anim here — the red tint from hurtTimer handles visual feedback,
            // and the anim priority logic in updateEnemy will show HURT only if not attacking
            if (e.hp <= 0) {
                e.isDead = true;
                e.deathTimer = 0.f;
                setEnemyAnim(e, ANIM_DEATH);
                gKills++;
                gScore += (e.type == ENEMY_GOBLIN) ? 100 : (e.type == ENEMY_DARKKNIGHT) ? 200 : 350;
                // drops
                if (rand() % 3 == 0) spawnPickup(e.pos, PICKUP_HEALTH);
                spawnPickup(e.pos + sf::Vector2f{20.f, 0.f}, PICKUP_COIN);
                spawnParticles(e.pos + sf::Vector2f{0,-60.f}, sf::Color(255,200,0), 10);
            }
        }
    }

    // Hit boss
    Boss& b = gBoss;
    if (b.active && !b.isDead) {
        sf::FloatRect bRect = makeRect(b.pos, b.width, b.height);
        if (rectsOverlap(attackBox, bRect) && b.hurtTimer <= 0.f) {
            b.hp        -= gPlayer.damage;
            b.hurtTimer  = 0.25f;
            spawnDamageText(b.pos + sf::Vector2f{0,-30.f}, gPlayer.damage, sf::Color::Cyan);
            spawnShake(6.f, 0.15f);
            // Red tint from hurtTimer is applied in draw; only switch to HURT anim if not attacking
            if (b.hp <= 0) {
                b.hp = 0;
                b.isDead = true;
                b.deathTimer = 0.f;
                setBossAnim(b, ANIM_DEATH);
                gScore += 2000;
                gKills++;
                spawnParticles(b.pos + sf::Vector2f{0,-100.f}, sf::Color(255,100,0), 20);
                spawnShake(20.f, 0.6f);
            }
        }
    }
}

// ============================================================
//  PICKUPS
// ============================================================
void updatePickups(float dt) {
    for (auto& pk : gPickups) {
        if (!pk.active) continue;
        pk.bobTimer += dt;
        sf::FloatRect pkRect = sf::FloatRect({ pk.pos.x - 15.f, pk.pos.y - 30.f + std::sin(pk.bobTimer * 3.f) * 5.f }, { 30.f, 30.f });
        sf::FloatRect pRect  = makeRect(gPlayer.pos, gPlayer.width, gPlayer.height);
        if (!gPlayer.isDead && rectsOverlap(pkRect, pRect)) {
            if (pk.type == PICKUP_HEALTH) {
                gPlayer.hp = std::min(gPlayer.hp + 100, gPlayer.maxHp);
                spawnDamageText(gPlayer.pos + sf::Vector2f{0,-40.f}, 100, sf::Color::Green);
            } else {
                gCoins++;
                gScore += 100;
                spawnDamageText(pk.pos + sf::Vector2f{0,-20.f}, 100, sf::Color::Yellow);
            }
            pk.active = false;
        }
    }
}

// ============================================================
//  DAMAGE TEXTS & PARTICLES
// ============================================================
void updateDamageTexts(float dt) {
    for (auto& dt2 : gDamageTexts) {
        if (!dt2.active) continue;
        dt2.pos.y   += dt2.velY * dt;
        dt2.velY    *= 0.95f;
        dt2.lifetime -= dt;
        dt2.alpha    = std::max(0.f, dt2.lifetime * 255.f);
        if (dt2.lifetime <= 0.f) dt2.active = false;
    }
}

void updateParticles(float dt) {
    for (auto& p : gParticles) {
        if (!p.active) continue;
        p.vel.y  += 500.f * dt;
        p.pos    += p.vel * dt;
        p.lifetime -= dt;
        if (p.lifetime <= 0.f) p.active = false;
    }
}

// ============================================================
//  CAMERA
// ============================================================
void updateCamera(float dt) {
    float curWorldW = LEVEL_CONFIGS[gCurrentLevel].width;
    gCamTargetX = gPlayer.pos.x - WINDOW_W * 0.35f;
    gCamTargetX = std::max(0.f, std::min(gCamTargetX, curWorldW - WINDOW_W));
    gCamX = lerp(gCamX, gCamTargetX, 1.0f - std::pow(0.01f, dt));
}

// ============================================================
//  LEVEL TRANSITION
// ============================================================
void checkLevelCompletion() {
    // Level is complete when all enemies (and boss if present) are dead
    bool allEnemiesDead = true;
    for (auto& e : gEnemies) {
        if (e.active && !e.isDead) { allEnemiesDead = false; break; }
    }

    bool bossDead = !LEVEL_CONFIGS[gCurrentLevel].boss || (gBoss.isDead && !gBoss.active);
    // for the boss level we wait for STATE_VICTORY from boss update
    if (LEVEL_CONFIGS[gCurrentLevel].boss) return;

    if (allEnemiesDead) {
        int next = gCurrentLevel + 1;
        if (next >= 4) {
            // shouldn't happen — boss level triggers victory
        } else {
            gCurrentLevel = next;
            buildLevel(gCurrentLevel);
        }
    }
}

// ============================================================
//  DRAW BACKGROUND (tiled)
// ============================================================
void drawBackground(sf::RenderWindow& win) {
    sf::Texture* bgTex = &texBg1;
    const char* key = LEVEL_CONFIGS[gCurrentLevel].bgKey;
    if (std::string(key) == "bg2") bgTex = &texBg2;
    if (std::string(key) == "bg3") bgTex = &texBg3;

    float bgW = (float)bgTex->getSize().x;
    float bgH = (float)bgTex->getSize().y;
    float scaleY = WINDOW_H / bgH;
    float scaleX = WINDOW_W / bgW;
    float usedScale = std::max(scaleX, scaleY);

    // Parallax: background scrolls at 30% of camera
    float bgScrollX = gCamX * 0.3f;

    sf::Sprite bg(*bgTex);
    bg.setScale({ usedScale, usedScale });

    float totalW = bgW * usedScale;
    float startX = -std::fmod(bgScrollX, totalW);
    for (float x = startX; x < WINDOW_W; x += totalW) {
        bg.setPosition({ x, 0.f });
        win.draw(bg);
    }
}

// ============================================================
//  DRAW UI
// ============================================================
void drawText(sf::RenderWindow& win, const std::string& str, float x, float y,
              unsigned size, sf::Color col = sf::Color::White) {
    sf::Text t(gFont, str, size);
    t.setFillColor(col);
    t.setPosition({ x, y });
    t.setOutlineThickness(1.5f);
    t.setOutlineColor(sf::Color::Black);
    win.draw(t);
}

void drawBar(sf::RenderWindow& win, float x, float y, float w, float h,
             float pct, sf::Color fill, sf::Color back = sf::Color(40,40,40,200)) {
    sf::RectangleShape bg({ w, h });
    bg.setPosition({ x, y });
    bg.setFillColor(back);
    win.draw(bg);
    sf::RectangleShape bar({ w * pct, h });
    bar.setPosition({ x, y });
    bar.setFillColor(fill);
    win.draw(bar);
    // Border
    sf::RectangleShape border({ w, h });
    border.setPosition({ x, y });
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(200,200,200,180));
    border.setOutlineThickness(1.5f);
    win.draw(border);
}

void drawHUD(sf::RenderWindow& win) {
    // Player HP bar
    float hpPct = (float)gPlayer.hp / (float)gPlayer.maxHp;
    drawBar(win, 20.f, 20.f, 220.f, 22.f, hpPct, sf::Color(220, 50, 50));
    drawText(win, "HP  " + intToStr(gPlayer.hp) + " / " + intToStr(gPlayer.maxHp),
             28.f, 20.f, 14, sf::Color::White);

    // Level name
    drawText(win, LEVEL_CONFIGS[gCurrentLevel].name, 20.f, 55.f, 16, sf::Color(255,220,80));

    // Score / Kills / Coins
    drawText(win, "Score: " + intToStr(gScore), 20.f, 80.f, 14, sf::Color::White);
    drawText(win, "Kills: " + intToStr(gKills),  20.f, 98.f, 14, sf::Color::White);
    drawText(win, "Coins: " + intToStr(gCoins),  20.f, 116.f,14, sf::Color::White);

    // Controls hint (bottom)
    drawText(win, "A/D - Move   W/Space - Jump   Z/J/Ctrl - Attack   Esc - Pause",
             WINDOW_W * 0.5f - 270.f, WINDOW_H - 22.f, 12, sf::Color(200,200,200,180));

    // Boss HP bar (top-center when boss is active and not dead)
    Boss& boss = gBoss;
    if (boss.active && !boss.isDead) {
        float bPct = (float)boss.hp / (float)boss.maxHp;
        drawBar(win, WINDOW_W * 0.3f, 20.f, WINDOW_W * 0.4f, 26.f, bPct, sf::Color(180, 0, 220));
        std::string phaseName = "DARK EMPEROR";
        if (boss.phase == 2) phaseName += "  [ Phase II ]";
        if (boss.phase == 3) phaseName += "  [ RAGE ]";
        drawText(win, phaseName, WINDOW_W * 0.3f + 10.f, 20.f, 14, sf::Color(255,180,255));
        drawText(win, intToStr(boss.hp) + " / " + intToStr(boss.maxHp),
                 WINDOW_W * 0.5f - 40.f, 48.f, 13, sf::Color::White);
    }
}

// ============================================================
//  DRAW GAME WORLD
// ============================================================
void drawGame(sf::RenderWindow& win) {
    // Determine camera offset + shake
    float shakeDx = 0.f, shakeDy = 0.f;
    if (gShakeTimer > 0.f) {
        shakeDx = ((rand() % 100 - 50) / 50.f) * gShakeAmount;
        shakeDy = ((rand() % 100 - 50) / 50.f) * gShakeAmount;
    }
    float camOff = -(gCamX + shakeDx);

    drawBackground(win);

    // Ground line
    sf::RectangleShape ground({ LEVEL_CONFIGS[gCurrentLevel].width, 10.f });
    ground.setPosition({ camOff, GROUND_Y });
    ground.setFillColor(sf::Color(40, 30, 20, 200));
    win.draw(ground);

    // Draw pickups
    for (auto& pk : gPickups) {
        if (!pk.active) continue;
        float bobY = std::sin(pk.bobTimer * 3.f) * 5.f;
        sf::CircleShape c(14.f);
        c.setFillColor(pk.type == PICKUP_HEALTH ? sf::Color(60, 200, 60) : sf::Color(255, 215, 0));
        c.setPosition({ pk.pos.x + camOff - 14.f, pk.pos.y - 28.f + bobY });
        win.draw(c);
        drawText(win, pk.type == PICKUP_HEALTH ? "+" : "$",
                 pk.pos.x + camOff - 7.f, pk.pos.y - 32.f + bobY, 16,
                 pk.type == PICKUP_HEALTH ? sf::Color::White : sf::Color::Yellow);
    }

    // Draw enemies
    for (auto& e : gEnemies) {
        if (!e.active) continue;

        // Select texture + SheetInfo for the current anim state
        sf::Texture* tex = &getEnemyTexIdle(e.type);
        SheetInfo si = (e.type == ENEMY_GOBLIN)     ? GOBLIN_IDLE_INFO :
                       (e.type == ENEMY_DARKKNIGHT) ? KNIGHT_IDLE_INFO : CMD_IDLE_INFO;

        if (e.animState == ANIM_RUN) {
            tex = &getEnemyTexRun(e.type);
            si  = (e.type == ENEMY_GOBLIN)     ? GOBLIN_RUN_INFO :
                  (e.type == ENEMY_DARKKNIGHT) ? KNIGHT_RUN_INFO : CMD_RUN_INFO;
        } else if (e.animState == ANIM_ATTACK) {
            tex = &getEnemyTexAttack(e.type);
            si  = (e.type == ENEMY_GOBLIN)     ? GOBLIN_ATTACK_INFO :
                  (e.type == ENEMY_DARKKNIGHT) ? KNIGHT_ATTACK_INFO : CMD_ATTACK_INFO;
        }

        // Use TARGET_H / si.charH so the on-screen character body stays the same size
        // across idle (369px frame, charH~330), run (184px frame, charH~155) and
        // attack sheets. Without this, idle frames render ~2x taller than run frames.
        float targetH = (e.type == ENEMY_GOBLIN)     ? GOBLIN_TARGET_H :
                        (e.type == ENEMY_DARKKNIGHT) ? KNIGHT_TARGET_H : CMD_TARGET_H;
        float scale   = targetH / static_cast<float>(si.charH);

        sf::Color tint = (e.hurtTimer > 0.f) ? sf::Color(255, 120, 120) : sf::Color::White;

        sf::Vector2f drawPos = { e.pos.x + camOff, e.pos.y };
        drawSprite(win, *tex, e.anim, drawPos, e.facingRight, scale, tint, si.cols, si.rows);

        // HP bar anchored to consistent character-top regardless of frame padding
        if (!e.isDead) {
            float epct   = (float)e.hp / (float)e.maxHp;
            float barTop = drawPos.y - targetH - 10.f;
            drawBar(win, drawPos.x - 30.f, barTop, 60.f, 6.f, epct, sf::Color(220, 50, 50));
        }
    }

    // Draw boss
    if (gBoss.active) {
        Boss& b = gBoss;
        sf::Texture* tex = &texBossIdle;
        SheetInfo bsi = BOSS_IDLE_INFO;
        if (b.animState == ANIM_RUN)    { tex = &texBossRun;    bsi = BOSS_RUN_INFO;    }
        if (b.animState == ANIM_ATTACK) { tex = &texBossAttack; bsi = BOSS_ATTACK_INFO; }

        sf::Color tint = sf::Color::White;
        if (b.hurtTimer > 0.f)  tint = sf::Color(255, 120, 120);
        if (b.rageFlash > 0.f)  tint = sf::Color(255, 50, 50);

        // TARGET / charH — idle charH=342, run charH=127, attack charH=157 (all measured)
        float bscale = BOSS_TARGET_H / static_cast<float>(bsi.charH);
        sf::Vector2f bDrawPos = { b.pos.x + camOff, b.pos.y };
        drawSprite(win, *tex, b.anim, bDrawPos, b.facingRight, bscale, tint, bsi.cols, bsi.rows);
    }

    // Draw princess (after boss death)
    if (gPrincessVisible) {
        Animation princessAnim;
        initAnim(princessAnim, 0, 0, 6, 0.15f, true);
        sf::Vector2f pDrawPos = { gPrincessPos.x + camOff, gPrincessPos.y };
        // Princess sheet is not in SheetInfo list, so pass 0 for charH (will use frameH as fallback)
        drawSprite(win, texPrincess, princessAnim, pDrawPos, true, SPRITE_SCALE);
    }

    // Draw player
    {
        sf::Texture* tex = &texHeroIdle;
        SheetInfo hsi = HERO_IDLE_INFO;

        if (gPlayer.animState == ANIM_RUN) {
            tex = &texHeroRun;
            hsi = HERO_RUN_INFO;
        } else if (gPlayer.animState == ANIM_ATTACK) {
            tex = &texHeroAttack;
            hsi = HERO_ATTACK_INFO;
        }
        // HURT and DEATH fall back to idle sheet (single frame)

        sf::Color tint = sf::Color::White;
        if (gPlayer.hurtTimer > 0.f) tint = sf::Color(255, 100, 100);
        // Invincibility flicker
        if (gPlayer.invincTimer > 0.f && (int)(gPlayer.invincTimer * 10) % 2 == 0)
            tint.a = 100;

        // TARGET / charH: hero always renders at HERO_TARGET_H character pixels tall
        float heroScale = HERO_TARGET_H / static_cast<float>(hsi.charH);

        sf::Vector2f pDrawPos = { gPlayer.pos.x + camOff, gPlayer.pos.y };
        drawSprite(win, *tex, gPlayer.anim, pDrawPos, gPlayer.facingRight,
                   heroScale, tint, hsi.cols, hsi.rows);

        // Attack hitbox debug (optional — uncomment to debug)
        // sf::RectangleShape hb({ gPlayer.attackBox.width, gPlayer.attackBox.height });
        // hb.setPosition({ gPlayer.attackBox.left + camOff, gPlayer.attackBox.top });
        // hb.setFillColor(sf::Color(255,255,0,80));
        // win.draw(hb);
    }

    // Particles
    for (auto& p : gParticles) {
        if (!p.active) continue;
        sf::CircleShape c(4.f);
        float alpha = (p.lifetime / p.maxLife) * 255.f;
        c.setFillColor(sf::Color(p.color.r, p.color.g, p.color.b, static_cast<uint8_t>(alpha)));
        c.setPosition({ p.pos.x + camOff - 4.f, p.pos.y - 4.f });
        win.draw(c);
    }

    // Damage texts (screen space — offset by camera)
    for (auto& dt2 : gDamageTexts) {
        if (!dt2.active) continue;
        sf::Color col = dt2.color;
        col.a = static_cast<uint8_t>(dt2.alpha);
        drawText(win, intToStr(dt2.value),
                 dt2.pos.x + camOff, dt2.pos.y, 18, col);
    }
}

// ============================================================
//  MAIN MENU DRAW
// ============================================================
void drawMainMenu(sf::RenderWindow& win) {
    // Dim background
    sf::Sprite bg(texBg3);
    float bscale = std::max((float)WINDOW_W / texBg3.getSize().x, (float)WINDOW_H / texBg3.getSize().y);
    bg.setScale({ bscale, bscale });
    bg.setColor(sf::Color(100, 100, 100));
    win.draw(bg);

    // Title
    drawText(win, "THE FALLEN KINGDOM", WINDOW_W * 0.5f - 200.f, 140.f, 42, sf::Color(255, 200, 50));
    drawText(win, "A Kingdom Lost. A Warrior Rises.", WINDOW_W * 0.5f - 165.f, 198.f, 18, sf::Color(200,200,200));

    const char* options[] = { "START GAME", "STORY", "EXIT" };
    for (int i = 0; i < 3; i++) {
        bool sel = (gMenuCursor == i);
        sf::Color col = sel ? sf::Color(255, 220, 60) : sf::Color(180, 180, 180);
        float x = WINDOW_W * 0.5f - 80.f;
        float y = 310.f + i * 60.f;
        if (sel) drawText(win, "> " + std::string(options[i]) + " <", x - 40.f, y, 24, col);
        else     drawText(win, options[i], x, y, 22, col);
    }

    drawText(win, "Use Arrow Keys / Enter to select", WINDOW_W * 0.5f - 145.f, WINDOW_H - 40.f, 14, sf::Color(150,150,150));
}

// ============================================================
//  STORY SCREEN DRAW
// ============================================================
void drawStory(sf::RenderWindow& win) {
    sf::RectangleShape bg({ (float)WINDOW_W, (float)WINDOW_H });
    bg.setFillColor(sf::Color(10, 8, 20));
    win.draw(bg);

    drawText(win, "THE STORY OF ELDORIA", WINDOW_W * 0.5f - 155.f, 40.f, 28, sf::Color(255, 200, 50));

    for (int i = 0; i < STORY_LINE_COUNT; i++) {
        drawText(win, STORY_LINES[i], WINDOW_W * 0.5f - 220.f, 100.f + i * 32.f, 18, sf::Color(220, 220, 220));
    }
}

// ============================================================
//  PAUSE SCREEN
// ============================================================
void drawPause(sf::RenderWindow& win) {
    sf::RectangleShape overlay({ (float)WINDOW_W, (float)WINDOW_H });
    overlay.setFillColor(sf::Color(0, 0, 0, 140));
    win.draw(overlay);
    drawText(win, "PAUSED", WINDOW_W * 0.5f - 60.f, WINDOW_H * 0.5f - 50.f, 40, sf::Color(255, 220, 60));
    drawText(win, "Press ESC or P to Resume", WINDOW_W * 0.5f - 145.f, WINDOW_H * 0.5f + 20.f, 20, sf::Color::White);
    drawText(win, "Press M for Main Menu",    WINDOW_W * 0.5f - 115.f, WINDOW_H * 0.5f + 55.f, 20, sf::Color(200,200,200));
}

// ============================================================
//  VICTORY / DEFEAT SCREENS
// ============================================================
void drawVictory(sf::RenderWindow& win) {
    sf::Sprite s(texWin);
    float sx = (float)WINDOW_W / texWin.getSize().x;
    float sy = (float)WINDOW_H / texWin.getSize().y;
    s.setScale({ sx, sy });
    win.draw(s);

    sf::RectangleShape overlay({ (float)WINDOW_W, (float)WINDOW_H });
    overlay.setFillColor(sf::Color(0, 0, 0, 120));
    win.draw(overlay);

    drawText(win, "KINGDOM SAVED!", WINDOW_W * 0.5f - 140.f, 100.f, 44, sf::Color(255, 220, 50));
    drawText(win, "The Princess has been rescued.", WINDOW_W * 0.5f - 170.f, 165.f, 22, sf::Color(220,220,220));

    drawText(win, "Final Score: " + intToStr(gScore), WINDOW_W * 0.5f - 100.f, 250.f, 22, sf::Color::Yellow);
    drawText(win, "Kills: " + intToStr(gKills),        WINDOW_W * 0.5f - 100.f, 282.f, 22, sf::Color::White);
    drawText(win, "Coins: " + intToStr(gCoins),        WINDOW_W * 0.5f - 100.f, 314.f, 22, sf::Color::White);

    drawText(win, "Press ENTER to return to Main Menu", WINDOW_W * 0.5f - 200.f, WINDOW_H - 60.f, 20, sf::Color(200,200,200));
}

void drawDefeat(sf::RenderWindow& win) {
    sf::Sprite s(texLoss);
    float sx = (float)WINDOW_W / texLoss.getSize().x;
    float sy = (float)WINDOW_H / texLoss.getSize().y;
    s.setScale({ sx, sy });
    win.draw(s);

    sf::RectangleShape overlay({ (float)WINDOW_W, (float)WINDOW_H });
    overlay.setFillColor(sf::Color(0, 0, 0, 130));
    win.draw(overlay);

    drawText(win, "THE KINGDOM HAS FALLEN", WINDOW_W * 0.5f - 195.f, 100.f, 40, sf::Color(220, 50, 50));
    drawText(win, "The last warrior has been defeated.", WINDOW_W * 0.5f - 185.f, 162.f, 22, sf::Color(200,200,200));
    drawText(win, "Score: " + intToStr(gScore), WINDOW_W * 0.5f - 60.f, 240.f, 22, sf::Color::Yellow);

    drawText(win, "Press ENTER to try again", WINDOW_W * 0.5f - 140.f, WINDOW_H - 100.f, 22, sf::Color(255,180,180));
    drawText(win, "Press M for Main Menu",    WINDOW_W * 0.5f - 120.f, WINDOW_H - 65.f,  20, sf::Color(200,200,200));
}

// ============================================================
//  SPLASH SCREEN
// ============================================================
void drawSplash(sf::RenderWindow& win) {
    sf::RectangleShape bg({ (float)WINDOW_W, (float)WINDOW_H });
    bg.setFillColor(sf::Color(5, 5, 20));
    win.draw(bg);

    float alpha = 255.f;
    if (gSplashTimer < 0.8f)       alpha = (gSplashTimer / 0.8f) * 255.f;
    if (gSplashTimer > 2.2f)       alpha = std::max(0.f, (3.0f - gSplashTimer) / 0.8f * 255.f);

    sf::Color tc = sf::Color(255, 220, 50, static_cast<uint8_t>(alpha));
    drawText(win, "GROWURK GAMES", WINDOW_W * 0.5f - 110.f, WINDOW_H * 0.5f - 30.f, 36, tc);
    drawText(win, "Presents", WINDOW_W * 0.5f - 45.f, WINDOW_H * 0.5f + 20.f, 22, sf::Color(180,180,180,static_cast<uint8_t>(alpha)));
}

// ============================================================
//  MAIN GAME UPDATE
// ============================================================
void updatePlaying(float dt) {
    updatePlayer(dt);
    for (auto& e : gEnemies) updateEnemy(e, dt);
    updateBoss(dt);
    checkPlayerAttackHits();
    updatePickups(dt);
    updateDamageTexts(dt);
    updateParticles(dt);
    updateCamera(dt);

    if (gShakeTimer > 0.f) gShakeTimer -= dt;
    else gShakeAmount = 0.f;

    // Activate boss when player gets close
    if (!gBoss.active && LEVEL_CONFIGS[gCurrentLevel].boss) {
        if (gPlayer.pos.x > LEVEL_CONFIGS[gCurrentLevel].width - 900.f) {
            gBoss.active = true;
        }
    }

    if (!LEVEL_CONFIGS[gCurrentLevel].boss) checkLevelCompletion();
}

// ============================================================
//  MAIN FUNCTION
// ============================================================
void darkKnight() {
    srand((unsigned)time(nullptr));

    sf::RenderWindow window(sf::VideoMode({ (unsigned)WINDOW_W, (unsigned)WINDOW_H }),
                            "The Fallen Kingdom",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    loadAssets();
    setPlayerAnim(gPlayer, ANIM_IDLE);

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;  // cap delta

        // ---- EVENT HANDLING ----
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                switch (gState) {
                    case STATE_SPLASH:
                        gState = STATE_MENU;
                        break;

                    case STATE_MENU:
                        if (kp->code == sf::Keyboard::Key::Up || kp->code == sf::Keyboard::Key::W)
                            gMenuCursor = (gMenuCursor + 2) % 3;
                        if (kp->code == sf::Keyboard::Key::Down || kp->code == sf::Keyboard::Key::S)
                            gMenuCursor = (gMenuCursor + 1) % 3;
                        if (kp->code == sf::Keyboard::Key::Enter || kp->code == sf::Keyboard::Key::Z) {
                            if (gMenuCursor == 0) startGame();
                            if (gMenuCursor == 1) gState = STATE_STORY;
                            if (gMenuCursor == 2) window.close();
                        }
                        break;

                    case STATE_STORY:
                        if (kp->code == sf::Keyboard::Key::Enter || kp->code == sf::Keyboard::Key::Escape)
                            gState = STATE_MENU;
                        break;

                    case STATE_PLAYING:
                        if (kp->code == sf::Keyboard::Key::Escape || kp->code == sf::Keyboard::Key::P)
                            gState = STATE_PAUSE;
                        break;

                    case STATE_PAUSE:
                        if (kp->code == sf::Keyboard::Key::Escape || kp->code == sf::Keyboard::Key::P)
                            gState = STATE_PLAYING;
                        if (kp->code == sf::Keyboard::Key::M)
                            gState = STATE_MENU;
                        break;

                    case STATE_VICTORY:
                        if (kp->code == sf::Keyboard::Key::Enter) {
                            soundBgm.stop();
                            bgmPlaying = false;
                            gState = STATE_MENU;
                        }
                        break;

                    case STATE_DEFEAT:
                        if (kp->code == sf::Keyboard::Key::Enter) {
                            soundBgm.stop();
                            bgmPlaying = false;
                            startGame();
                        }
                        if (kp->code == sf::Keyboard::Key::M) {
                            soundBgm.stop();
                            bgmPlaying = false;
                            gState = STATE_MENU;
                        }
                        break;
                }
            }
        }

        // ---- UPDATE ----
        switch (gState) {
            case STATE_SPLASH:
                gSplashTimer += dt;
                if (gSplashTimer > 3.0f) gState = STATE_MENU;
                break;
            case STATE_PLAYING:
                updatePlaying(dt);
                break;
            default: break;
        }

        // ---- DRAW ----
        window.clear(sf::Color(20, 15, 30));

        switch (gState) {
            case STATE_SPLASH:  drawSplash(window);  break;
            case STATE_MENU:    drawMainMenu(window); break;
            case STATE_STORY:   drawStory(window);    break;
            case STATE_PLAYING:
                drawGame(window);
                drawHUD(window);
                break;
            case STATE_PAUSE:
                drawGame(window);
                drawHUD(window);
                drawPause(window);
                break;
            case STATE_VICTORY: drawVictory(window); break;
            case STATE_DEFEAT:  drawDefeat(window);  break;
        }

        window.display();
    }

    // Stop background music when exiting
    if (bgmPlaying) {
        soundBgm.stop();
        bgmPlaying = false;
    }

    return;
}