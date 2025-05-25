#include "gameplay/PrairieKing.hpp"
#include "discord/DiscordManager.hpp"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <cmath>

// Static member initialization
int PrairieKing::m_world = 0;

// Static game instance pointer for accessing from static methods
static PrairieKing *s_instance = nullptr;

// Initialize CowboyPowerup implementation
PrairieKing::CowboyPowerup::CowboyPowerup(int which, Vector2 position, int duration)
    : which(which), position(position), duration(duration), yOffset(0.0f)
{
}

void PrairieKing::CowboyPowerup::Draw(const Texture2D &cursorTexture, Vector2 topLeftScreenCoordinate) const
{
    if (duration > 2000 || duration / 200 % 2 == 0)
    {
        Rectangle source = {144.0f + which * 16.0f, 160.0f, 16.0f, 16.0f};
        DrawTexturePro(
            cursorTexture,
            source,
            Rectangle{topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y + yOffset, 16.0f * 3, 16.0f * 3},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }
}

// Initialize JOTPKProgress implementation
PrairieKing::JOTPKProgress::JOTPKProgress()
    : bulletDamage(0), fireSpeedLevel(0), ammoLevel(0), spreadPistol(false),
      runSpeedLevel(0), lives(0), coins(0), score(0), died(false),
      whichRound(0), whichWave(0), heldItem(-100), world(0), waveTimer(0)
{
}

// Initialize CowboyBullet implementation
PrairieKing::CowboyBullet::CowboyBullet(Vector2 position, Vector2 motion, int damage)
    : position(position), motion(motion), damage(damage)
{
}

PrairieKing::CowboyBullet::CowboyBullet(Vector2 position, int direction, int damage)
    : position(position), damage(damage)
{
    switch (direction)
    {
    case 0: // Up
        motion = {0, -BULLET_SPEED};
        break;
    case 1: // Right
        motion = {BULLET_SPEED, 0};
        break;
    case 2: // Down
        motion = {0, BULLET_SPEED};
        break;
    case 3: // Left
        motion = {-BULLET_SPEED, 0};
        break;
    }
}

// Initialize TemporaryAnimatedSprite implementation
PrairieKing::TemporaryAnimatedSprite::TemporaryAnimatedSprite(
    Rectangle sourceRect, float interval, int frameCount, int startFrame,
    Vector2 pos, float rot, float scale, bool flip, float depth, Color color)
    : sourceRect(sourceRect), position(pos), animationInterval(interval),
      frames(frameCount), currentFrame(startFrame), timer(0), rotation(rot),
      scale(scale), flipped(flip), layerDepth(depth), tint(color),
      delayBeforeAnimationStart(0), extraData(0), alpha(1.0f)
{
}

bool PrairieKing::TemporaryAnimatedSprite::Update(float deltaTime)
{
    if (delayBeforeAnimationStart > 0)
    {
        delayBeforeAnimationStart -= static_cast<int>(deltaTime * 1000);
        return false;
    }

    timer += deltaTime * 1000.0f;

    if (timer >= animationInterval)
    {
        currentFrame++;
        timer = 0;

        if (currentFrame >= frames)
        {
            if (endFunction)
            {
                endFunction(extraData);
            }
            return true;
        }
    }
    return false;
}

void PrairieKing::TemporaryAnimatedSprite::Draw(const Texture2D &texture)
{
    if (delayBeforeAnimationStart > 0)
        return;

    Rectangle source = sourceRect;
    source.x += source.width * currentFrame;

    DrawTexturePro(
        texture,
        source,
        Rectangle{position.x, position.y, source.width * scale, source.height * scale},
        Vector2{0, 0},
        rotation,
        tint);
}

// Main PrairieKing class implementation
PrairieKing::PrairieKing(AssetManager &assets)
    : m_assets(assets),
      m_isGameOver(false),
      m_gameOver(false),
      m_quit(false),
      m_died(false),
      m_shopping(false),
      m_gopherRunning(false),
      m_store(false),
      m_merchantLeaving(false),
      m_merchantArriving(false),
      m_merchantShopOpen(false),
      m_waitingForPlayerToMoveDownAMap(false),
      m_scrollingMap(false),
      m_hasGopherAppeared(false),
      m_shootoutLevel(false),
      m_gopherTrain(false),
      m_playerJumped(false),
      m_endCutscene(false),
      m_spreadPistol(false),
      m_runSpeedLevel(0),
      m_fireSpeedLevel(0),
      m_ammoLevel(0),
      m_whichRound(0),
      m_bulletDamage(1),
      m_speedBonus(0),
      m_fireRateBonus(0),
      m_lives(3),
      m_coins(0),
      m_score(0),
      m_shootingDelay(300),
      m_shotTimer(0),
      m_motionPause(0),
      m_gameOverOption(0),
      m_gameRestartTimer(0),
      m_fadeThenQuitTimer(0),
      m_whichWave(0),
      m_monsterConfusionTimer(0),
      m_zombieModeTimer(0),
      m_shoppingTimer(0),
      m_holdItemTimer(0),
      m_itemToHold(-1),
      m_newMapPosition(0),
      m_playerInvincibleTimer(0),
      m_screenFlash(0),
      m_gopherTrainPosition(0),
      m_endCutsceneTimer(0),
      m_endCutscenePhase(0),
      m_deathTimer(0),
      m_waveTimer(WAVE_DURATION),
      m_betweenWaveTimer(BETWEEN_WAVE_DURATION),
      m_heldItem(nullptr),
      m_cactusDanceTimer(0),
      m_playerMotionAnimationTimer(0),
      m_playerFootstepSoundTimer(200.0f),
      m_spawnQueue(4),
      m_overworldSong({0}),
      m_debugMode(false),
      m_isPaused(false)
{
    // Store the instance pointer for static access
    s_instance = this;

    // Initialize the game
    Initialize();
}

void PrairieKing::Initialize()
{
    // Initialize game state
    m_whichWave = 0;
    m_betweenWaveTimer = GameConstants::BETWEEN_WAVE_DURATION;
    m_waveTimer = 0;
    m_world = DESERT_WORLD;
    m_lives = 3;
    m_coins = 0;
    m_score = 0;
    m_bulletDamage = 1;
    m_fireSpeedLevel = 0;
    m_ammoLevel = 0;
    m_runSpeedLevel = 0;
    m_spreadPistol = false;
    m_waveCompleted = false;

    // Center the screen
    m_topLeftScreenCoordinate = Vector2{
        static_cast<float>(GetScreenWidth()) / 2.0f - 384.0f,
        static_cast<float>(GetScreenHeight()) / 2.0f - 384.0f};

    // Initialize monster chances
    m_monsterChances.clear();
    m_monsterChances.push_back(Vector2{0.0f, 0.0f}); // Orc
    m_monsterChances.push_back(Vector2{0.0f, 0.0f}); // Ghost
    m_monsterChances.push_back(Vector2{0.0f, 0.0f}); // Ogre
    m_monsterChances.push_back(Vector2{0.0f, 0.0f}); // Mummy
    m_monsterChances.push_back(Vector2{0.0f, 0.0f}); // Devil
    m_monsterChances.push_back(Vector2{0.0f, 0.0f}); // Mushroom
    m_monsterChances.push_back(Vector2{0.0f, 0.0f}); // Spikey

    // Load music
    m_overworldSong = m_assets.GetMusic("overworld");

    // Initialize map
    GetMap(0, m_map);
    memcpy(m_nextMap, m_map, sizeof(m_map));

    // Initialize player position
    m_playerPosition = Vector2{static_cast<float>(8 * GetTileSize()), static_cast<float>(8 * GetTileSize())};
    m_playerBoundingBox = Rectangle{
        m_playerPosition.x + static_cast<float>(GetTileSize()) / 4.0f,
        m_playerPosition.y + static_cast<float>(GetTileSize()) / 4.0f,
        static_cast<float>(GetTileSize()) / 2.0f,
        static_cast<float>(GetTileSize()) / 2.0f};

    // Initialize other game objects
    m_merchantBox = Rectangle{0.0f, 0.0f, 0.0f, 0.0f};
    m_noPickUpBox = Rectangle{0.0f, 0.0f, 0.0f, 0.0f};
    m_gopherBox = Rectangle{0.0f, 0.0f, 0.0f, 0.0f};
    m_shoppingCarpetNoPickup = Rectangle{0.0f, 0.0f, 0.0f, 0.0f};

    // Clear collections
    m_monsters.clear();
    m_bullets.clear();
    m_enemyBullets.clear();
    m_powerups.clear();
    m_temporarySprites.clear();
    m_playerMovementDirections.clear();
    m_playerShootingDirections.clear();
    m_storeItems.clear();
    m_activePowerups.clear();
    m_spawnQueue.clear();
    m_spawnQueue.resize(8);

    // Initialize input state
    m_buttonHeldState.clear();
    m_buttonHeldFrames.clear();
}

bool PrairieKing::LoadGame()
{
    // Por ahora, simplemente retornamos false ya que no implementaremos el guardado
    // En una implementación real, aquí cargaríamos el estado del juego desde un archivo
    return false;
}

void PrairieKing::SaveGame()
{
    // Por ahora, no hacemos nada ya que no implementaremos el guardado
    // En una implementación real, aquí guardaríamos el estado del juego en un archivo
}

void PrairieKing::Reset()
{
    // Reiniciar el juego a su estado inicial
    Initialize();

    // Reiniciar valores específicos
    m_lives = 3;
    m_coins = 0;
    m_score = 0;
    m_whichWave = 0;
    m_whichRound = 0;
    m_bulletDamage = 1;
    m_fireSpeedLevel = 0;
    m_ammoLevel = 0;
    m_runSpeedLevel = 0;
    m_spreadPistol = false;

    // Reiniciar el mapa
    GetMap(m_whichWave, m_map);

    // Reiniciar la posición del jugador
    m_playerPosition = {384.0f, 384.0f};
    m_playerBoundingBox = {
        m_playerPosition.x + static_cast<float>(GetTileSize()) / 4.0f,
        m_playerPosition.y + static_cast<float>(GetTileSize()) / 4.0f,
        static_cast<float>(GetTileSize()) / 2.0f,
        static_cast<float>(GetTileSize()) / 2.0f};
}

void PrairieKing::ApplyNewGamePlus()
{
    // Aumentar la dificultad para la siguiente ronda
    m_monsterChances[0] = {0.014f + static_cast<float>(m_whichRound) * 0.005f, 0.41f + static_cast<float>(m_whichRound) * 0.05f};
    m_monsterChances[4] = {0.002f, 0.1f};

    // Mantener el progreso del jugador
    // No reiniciamos las vidas, monedas, ni el score
    // Solo aumentamos la dificultad de los monstruos
}

void PrairieKing::ApplyLevelSpecificStates()
{
    if (m_whichWave == 12)
    {
        m_shootoutLevel = true;
         //m_monsters.push_back(new Dracula(m_assets));

        if (m_whichRound > 0)
        {
            m_monsters.back()->health *= 2;
        }
    }
    else if (m_whichWave > 0 && m_whichWave % 4 == 0)
    {
        m_shootoutLevel = true;
        //m_monsters.push_back(new Outlaw(m_assets, Vector2{static_cast<float>(8 * GetTileSize()), static_cast<float>(13 * GetTileSize())}, (m_world == 0) ? 50 : 100));

        // Play outlaw music
        PlaySound(GetSound("cowboy_outlawsong"));
    }
}

float PrairieKing::GetMovementSpeed(float speed, int directions)
{
    float movementSpeed = speed;
    if (directions > 1)
    {
        movementSpeed = std::max(1.0f, static_cast<float>(sqrt(2.0f * (movementSpeed * movementSpeed)) / 2.0f));
    }
    return movementSpeed;
}

bool PrairieKing::GetPowerUp(CowboyPowerup c)
{
    switch (c.which)
    {
    case POWERUP_HEART:
        UsePowerup(POWERUP_HEART);
        break;

    case POWERUP_SKULL:
        UsePowerup(POWERUP_SKULL);
        break;

    case POWERUP_LOG:
        UsePowerup(POWERUP_LOG);
        break;

    case COIN1:
        m_coins++;
        PlaySound(GetSound("pickup_coin"));
        break;

    case COIN5:
        m_coins += 5;
        PlaySound(GetSound("pickup_coin"));
        break;

    case POWERUP_LIFE:
        m_lives++;
        PlaySound(GetSound("cowboy_powerup"));
        break;

    default:
        if (!m_heldItem)
        {
            m_heldItem = std::make_unique<CowboyPowerup>(c);
            PlaySound(GetSound("cowboy_powerup"));
            break;
        }

        // Intercambiar power-up actual con el nuevo
        auto tmp = std::move(m_heldItem);
        m_heldItem = std::make_unique<CowboyPowerup>(c);
        m_noPickUpBox = {c.position.x, c.position.y, static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())};
        tmp->position = c.position;
        m_powerups.push_back(*tmp);
        PlaySound(GetSound("cowboy_powerup"));
        return true;
    }
    return true;
}

void PrairieKing::UsePowerup(int which)
{
    // Primero verificar si el powerup ya está activo
    if (m_activePowerups.find(which) != m_activePowerups.end())
    {
        m_activePowerups[which] = POWERUP_DURATION + 2000;
        return;
    }

    switch (which)
    {
    case POWERUP_HEART:
        m_itemToHold = 13;
        m_holdItemTimer = 4000;
        PlaySound(GetSound("Cowboy_Secret"));
        m_endCutscene = true;
        m_endCutsceneTimer = 4000;
        m_world = 0;
        break;

    case POWERUP_SKULL:
        m_itemToHold = 11;
        m_holdItemTimer = 2000;
        PlaySound(GetSound("Cowboy_Secret"));
        m_gopherTrain = true;
        m_gopherTrainPosition = -GetTileSize() * 2;
        break;

    case POWERUP_LOG:
        m_itemToHold = 12;
        m_holdItemTimer = 2000;
        PlaySound(GetSound("Cowboy_Secret"));
        m_gopherTrain = true;
        m_gopherTrainPosition = -GetTileSize() * 2;
        break;

    case POWERUP_SHERRIFF:
        UsePowerup(POWERUP_SHOTGUN);
        UsePowerup(POWERUP_RAPIDFIRE);
        UsePowerup(POWERUP_SPEED);
        for (auto &powerup : m_activePowerups)
        {
            powerup.second *= 2;
        }
        break;

    case POWERUP_ZOMBIE:
        // Stop current music
        StopMusicStream(m_overworldSong);

        // Play zombie music
        PlaySound(GetSound("Cowboy_undead"));

        // Set timers
        m_motionPause = 1800.0f;
        m_zombieModeTimer = 10000.0f;
        break;

    case POWERUP_TELEPORT:
    {
        Vector2 teleportSpot = {0, 0};
        int tries = 0;
        while ((fabsf(teleportSpot.x - m_playerPosition.x) < 8.0f ||
                fabsf(teleportSpot.y - m_playerPosition.y) < 8.0f ||
                IsCollidingWithMap(teleportSpot) ||
                IsCollidingWithMonster(Rectangle{teleportSpot.x, teleportSpot.y,
                                                 static_cast<float>(GetTileSize()),
                                                 static_cast<float>(GetTileSize())},
                                       nullptr)) &&
               tries < 10)
        {
            teleportSpot = {
                static_cast<float>(GetRandomFloat(GetTileSize(), 16 * GetTileSize() - GetTileSize())),
                static_cast<float>(GetRandomFloat(GetTileSize(), 16 * GetTileSize() - GetTileSize()))};
            tries++;
        }

        if (tries < 10)
        {
            // Add teleport visual effects at start and end positions
            // At player position
            m_temporarySprites.push_back(TemporaryAnimatedSprite(
                Rectangle{336, 144, 16, 16}, 120.0f, 5, 0,
                Vector2{m_playerPosition.x + m_topLeftScreenCoordinate.x + GetTileSize() / 2,
                        m_playerPosition.y + m_topLeftScreenCoordinate.y + GetTileSize() / 2},
                0.0f, 3.0f, false, 1.0f, WHITE));

            // At teleport destination
            m_temporarySprites.push_back(TemporaryAnimatedSprite(
                Rectangle{336, 144, 16, 16}, 120.0f, 5, 0,
                Vector2{teleportSpot.x + m_topLeftScreenCoordinate.x + GetTileSize() / 2,
                        teleportSpot.y + m_topLeftScreenCoordinate.y + GetTileSize() / 2},
                0.0f, 3.0f, false, 1.0f, WHITE));

            // Additional effects around destination
            auto addDelayedEffect = [this](Vector2 pos, int delay)
            {
                TemporaryAnimatedSprite sprite(
                    Rectangle{336, 144, 16, 16}, 120.0f, 5, 0,
                    Vector2{pos.x + m_topLeftScreenCoordinate.x + GetTileSize() / 2,
                            pos.y + m_topLeftScreenCoordinate.y + GetTileSize() / 2},
                    0.0f, 3.0f, false, 1.0f, WHITE);
                sprite.delayBeforeAnimationStart = delay;
                m_temporarySprites.push_back(sprite);
            };

            // Left
            addDelayedEffect(Vector2{teleportSpot.x - GetTileSize() / 2, teleportSpot.y}, 200);
            // Right
            addDelayedEffect(Vector2{teleportSpot.x + GetTileSize() / 2, teleportSpot.y}, 400);
            // Top
            addDelayedEffect(Vector2{teleportSpot.x, teleportSpot.y - GetTileSize() / 2}, 600);
            // Bottom
            addDelayedEffect(Vector2{teleportSpot.x, teleportSpot.y + GetTileSize() / 2}, 800);

            // Teleport player and apply effects
            m_playerPosition = teleportSpot;
            m_monsterConfusionTimer = 2000; // Reduced from 4000 to 2000
            m_playerInvincibleTimer = 2000; // Reduced from 4000 to 2000
            PlaySound(GetSound("cowboy_powerup"));
        }
        break;
    }

    case POWERUP_LIFE:
        m_lives++;
        PlaySound(GetSound("cowboy_powerup"));
        break;

    case POWERUP_NUKE:
        PlaySound(GetSound("cowboy_explosion"));
        if (!m_shootoutLevel)
        {
            for (auto monster : m_monsters)
            {
                AddGuts({monster->position.x, monster->position.y}, monster->type);
                delete monster;
            }
            m_monsters.clear();
        }
        else
        {
            for (auto monster : m_monsters)
            {
                monster->TakeDamage(30);
            }
        }

        for (int i = 0; i < 30; i++)
        {
            m_temporarySprites.push_back(TemporaryAnimatedSprite(
                {336, 144, 16, 16},
                80.0f, 5, 0,
                {static_cast<float>(GetRandomFloat(1, 16) * GetTileSize()) + m_topLeftScreenCoordinate.x,
                 static_cast<float>(GetRandomFloat(1, 16) * GetTileSize()) + m_topLeftScreenCoordinate.y},
                0.0f, 3.0f, false,
                1.0f, WHITE));
            m_temporarySprites.back().delayBeforeAnimationStart = static_cast<int>(GetRandomFloat(0, 800));
        }
        break;

    case POWERUP_SPREAD:
    case POWERUP_RAPIDFIRE:
    case POWERUP_SHOTGUN:
    case POWERUP_SPEED:
        m_shotTimer = 0;
        PlaySound(GetSound("cowboy_gunload"));
        m_activePowerups[which] = POWERUP_DURATION;
        break;

    case COIN1:
        m_coins++;
        PlaySound(GetSound("pickup_coin"));
        break;

    case COIN5:
        m_coins += 5;
        PlaySound(GetSound("pickup_coin"));
        break;

    default:
        m_activePowerups[which] = POWERUP_DURATION;
        PlaySound(GetSound("cowboy_powerup"));
        break;
    }

    // Reducir duración en New Game Plus
    if (m_whichRound > 0 && m_activePowerups.find(which) != m_activePowerups.end())
    {
        m_activePowerups[which] /= 2;
    }
}

void PrairieKing::AddGuts(Vector2 position, int whichGuts)
{
    PrairieKing *instance = GetGameInstance();
    if (!instance)
        return;

    // Create appropriate temporary sprites based on monster type
    switch (whichGuts)
    {
    case ORC:
    case OGRE:
    case MUSHROOM:
    case SPIKEY:
    case DRACULA:
    {
        // Blood splat animation
        TemporaryAnimatedSprite blood(
            Rectangle{384.0f, 48.0f, 16.0f, 16.0f},
            80.0f, 6, 0,
            {position.x + instance->m_topLeftScreenCoordinate.x, position.y + instance->m_topLeftScreenCoordinate.y},
            0.0f, 3.0f, GetRandomFloat(0, 1) < 0.5f,
            0.001f, WHITE);
        instance->AddTemporarySprite(blood);

        // Lingering guts
        TemporaryAnimatedSprite guts(
            Rectangle{464.0f, 48.0f, 16.0f, 16.0f},
            10000.0f, 1, 0,
            {position.x + instance->m_topLeftScreenCoordinate.x, position.y + instance->m_topLeftScreenCoordinate.y},
            0.0f, 3.0f, GetRandomFloat(0, 1) < 0.5f,
            0.001f, WHITE);
        guts.delayBeforeAnimationStart = 480;
        instance->AddTemporarySprite(guts);
        break;
    }

    case MUMMY:
    {
        // Mummy specific death animation
        TemporaryAnimatedSprite mummyDeath(
            Rectangle{336.0f, 144.0f, 16.0f, 16.0f},
            80.0f, 5, 0,
            {position.x + instance->m_topLeftScreenCoordinate.x, position.y + instance->m_topLeftScreenCoordinate.y},
            0.0f, 3.0f, GetRandomFloat(0, 1) < 0.5f,
            0.001f, WHITE);
        instance->AddTemporarySprite(mummyDeath);
        break;
    }

    case EVIL_BUTTERFLY:
    case IMP:
    {
        // Ghost/Devil specific death animation
        TemporaryAnimatedSprite ghostDeath(
            Rectangle{416.0f, 80.0f, 16.0f, 16.0f},
            80.0f, 4, 0,
            {position.x + instance->m_topLeftScreenCoordinate.x, position.y + instance->m_topLeftScreenCoordinate.y},
            0.0f, 3.0f, GetRandomFloat(0, 1) < 0.5f,
            0.001f, WHITE);
        instance->AddTemporarySprite(ghostDeath);
        break;
    }
    }
}

void PrairieKing::EndOfGopherAnimationBehavior2(int extraInfo)
{
    PlaySound(GetSound("cowboy_gopher"));

    if (fabsf(m_gopherBox.x - 8 * GetTileSize()) > fabsf(m_gopherBox.y - 8 * GetTileSize()))
    {
        if (m_gopherBox.x > 8 * GetTileSize())
        {
            m_gopherMotion = {-2, 0};
        }
        else
        {
            m_gopherMotion = {2, 0};
        }
    }
    else
    {
        if (m_gopherBox.y > 8 * GetTileSize())
        {
            m_gopherMotion = {0, -2};
        }
        else
        {
            m_gopherMotion = {0, 2};
        }
    }

    m_gopherRunning = true;
}

void PrairieKing::EndOfGopherAnimationBehavior(int extraInfo)
{
    m_temporarySprites.push_back(TemporaryAnimatedSprite(
        {256, 144, 16, 16},
        120.0f, 4, 2,
        {m_topLeftScreenCoordinate.x + m_gopherBox.x + GetTileSize() / 2,
         m_topLeftScreenCoordinate.y + m_gopherBox.y + GetTileSize() / 2},
        0.0f, 3.0f, false,
        static_cast<float>(m_gopherBox.y) / 10000.0f, WHITE));

    m_temporarySprites.back().endFunction = [this](int extraInfo)
    { EndOfGopherAnimationBehavior2(extraInfo); };
    PlaySound(GetSound("cowboy_gopher"));
}

void PrairieKing::KillOutlaw()
{
    PlaySound(GetSoundStatic("Cowboy_monsterDie"));
}

void PrairieKing::UpdateBullets(float deltaTime)
{
    // Update player bullets
    for (auto it = m_bullets.begin(); it != m_bullets.end();)
    {
        it->position.x += it->motion.x * BULLET_SPEED * deltaTime;
        it->position.y += it->motion.y * BULLET_SPEED * deltaTime;

        // Check collision with monsters
        bool hitMonster = false;
        for (auto monster : m_monsters)
        {
            if (monster->invisible)
                continue;

            Rectangle bulletRect = {
                it->position.x,
                it->position.y,
                static_cast<float>(GetTileSize() / 4),
                static_cast<float>(GetTileSize() / 4)};

            if (CheckCollisionRecs(bulletRect, monster->position))
            {
                if (monster->TakeDamage(it->damage))
                {
                    // Monster died, remove it
                    delete monster;
                    m_monsters.erase(std::remove(m_monsters.begin(), m_monsters.end(), monster), m_monsters.end());
                }
                hitMonster = true;
                break;
            }
        }

        // Check collision with map
        if (!hitMonster && IsCollidingWithMap(it->position))
        {
            hitMonster = true;
        }

        if (hitMonster)
        {
            it = m_bullets.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Update enemy bullets
    for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end();)
    {
        it->position.x += it->motion.x * BULLET_SPEED * deltaTime;
        it->position.y += it->motion.y * BULLET_SPEED * deltaTime;

        // Check collision with player
        Rectangle bulletRect = {
            it->position.x,
            it->position.y,
            static_cast<float>(GetTileSize() / 4),
            static_cast<float>(GetTileSize() / 4)};

        if (CheckCollisionRecs(bulletRect, m_playerBoundingBox) && m_playerInvincibleTimer <= 0)
        {
            PlayerDie();
            it = m_enemyBullets.erase(it);
        }
        else if (IsCollidingWithMap(it->position))
        {
            it = m_enemyBullets.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void PrairieKing::PlayerDie()
{
    PauseMusicStream(m_overworldSong);
    m_gopherRunning = false;
    m_hasGopherAppeared = false;

    // Reset spawn queues
    for (int i = 0; i < 4; i++)
    {
        m_spawnQueue[i].clear();
    }

    m_enemyBullets.clear();

    if (!m_shootoutLevel)
    {
        m_powerups.clear();

        // Clear monsters
        for (auto monster : m_monsters)
        {
            delete monster;
        }
        m_monsters.clear();
    }

    m_died = true;
    m_activePowerups.clear();
    m_deathTimer = DEATH_DELAY;

    // Reset the wave timer properly
    m_waveTimer = std::min(WAVE_DURATION, m_waveTimer + 10000);
    m_betweenWaveTimer = 4000;

    // Add death animation
    m_temporarySprites.push_back(TemporaryAnimatedSprite(
        Rectangle{336, 160, 16, 16},
        120.0f, 5, 0,
        Vector2{
            m_topLeftScreenCoordinate.x + m_playerPosition.x + GetTileSize(),
            m_topLeftScreenCoordinate.y + m_playerPosition.y + GetTileSize()},
        0.0f, 3.0f, false,
        1.0f, WHITE));

    m_betweenWaveTimer = 4000;

    // Lose a life
    m_lives--;
    m_playerInvincibleTimer = 5000;
    if (m_shootoutLevel)
    {
        m_playerPosition = Vector2{static_cast<float>(8 * GetTileSize()), static_cast<float>(8 * GetTileSize())};
        PlaySound(PrairieKing::GetSoundStatic("Cowboy_monsterDie"));
    }
    else
    {
        m_playerPosition = Vector2{static_cast<float>(8 * GetTileSize()), static_cast<float>(8 * GetTileSize())};
        m_playerBoundingBox = {
            m_playerPosition.x,                        // Sin offset en X
            m_playerPosition.y - GetTileSize() / 4.0f, // Offset negativo en Y para incluir la gorra
            static_cast<float>(GetTileSize()),         // Ancho completo del tile
            static_cast<float>(GetTileSize() * 1.25f)  // Alto aumentado para incluir pies y gorra
        };
        PlaySound(GetSound("cowboy_dead"));
    }

    if (m_lives < 0)
    {
        // Add death animation sprite
        m_temporarySprites.push_back(TemporaryAnimatedSprite(
            Rectangle{336, 160, 16, 16},
            550.0f, 5, 0,
            Vector2{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                    m_topLeftScreenCoordinate.y + m_playerPosition.y},
            0.0f, 3.0f, false,
            1.0f, WHITE));

        m_temporarySprites.back().alpha = 0.001f;
        m_temporarySprites.back().endFunction = [this](int extra)
        { AfterPlayerDeathFunction(extra); };

        m_deathTimer *= 3.0f;
        // Save game progress reset
        SaveGame();

        // Game over
        m_gameOver = true;
    }
}

void PrairieKing::AfterPlayerDeathFunction(int extra)
{
    if (m_lives < 0)
    {
        m_gameOver = true;

        // Stop music
        StopMusicStream(m_overworldSong);

        // Clear game objects
        for (auto monster : m_monsters)
        {
            delete monster;
        }
        m_monsters.clear();
        m_powerups.clear();
        m_died = false;

        PlaySound(GetSound("Cowboy_monsterDie"));
    }
}

void PrairieKing::StartNewRound()
{
    m_gameRestartTimer = 2000;
    PlaySound(PrairieKing::GetSoundStatic("Cowboy_monsterDie"));
    m_whichRound++;
}

void PrairieKing::ProcessInputs()
{
    if (m_gameOver)
    {
        // Handle game over menu navigation
        if (IsKeyDown(GameKeys::ShootUp))
        {
            m_gameOverOption = std::max(0, m_gameOverOption - 1);
            PlaySound(GetSound("Cowboy_gunshot"));
        }
        if (IsKeyDown(GameKeys::ShootDown))
        {
            m_gameOverOption = std::min(1, m_gameOverOption + 1);
            PlaySound(GetSound("Cowboy_gunshot"));
        }

        if (IsKeyDown(GameKeys::SelectOption))
        {
            if (m_gameOverOption == 1)
            {
                m_quit = true;
            }
            else
            {
                m_gameRestartTimer = 1500;
                m_gameOver = false;
                m_gameOverOption = 0;
                PlaySound(GetSound("pickup_coin"));
            }
        }
        return;
    }

    // Clear previous movement and shooting directions
    m_playerMovementDirections.clear();
    m_playerShootingDirections.clear();

    // Process movement inputs
    if (IsKeyDown(GameKeys::MoveUp))
    {
        if (m_gameOver && IsKeyPressed(GameKeys::MoveUp))
        {
            m_gameOverOption = std::max(0, m_gameOverOption - 1);
            PlaySound(GetSoundStatic("Cowboy_gunshot"));
        }
        AddPlayerMovementDirection(0);
    }

    if (IsKeyDown(GameKeys::MoveDown))
    {
        if (m_gameOver && IsKeyPressed(GameKeys::MoveDown))
        {
            m_gameOverOption = std::min(1, m_gameOverOption + 1);
            PlaySound(GetSoundStatic("Cowboy_gunshot"));
        }
        AddPlayerMovementDirection(2);
    }

    if (IsKeyDown(GameKeys::MoveLeft))
    {
        AddPlayerMovementDirection(3);
    }

    if (IsKeyDown(GameKeys::MoveRight))
    {
        AddPlayerMovementDirection(1);
    }

    // Process shooting inputs
    if (IsKeyDown(GameKeys::ShootUp))
    {
        AddPlayerShootingDirection(0);
    }

    if (IsKeyDown(GameKeys::ShootDown))
    {
        AddPlayerShootingDirection(2);
    }

    if (IsKeyDown(GameKeys::ShootLeft))
    {
        AddPlayerShootingDirection(3);
    }

    if (IsKeyDown(GameKeys::ShootRight))
    {
        AddPlayerShootingDirection(1);
    }

    if (IsKeyDown(GameKeys::UsePowerup) && !m_gameOver && m_heldItem != nullptr)
    {
        std::cout << "Attempting to use powerup..." << std::endl;
        if (m_deathTimer <= 0.0f && m_zombieModeTimer <= 0)
        {
            std::cout << "Using powerup: " << m_heldItem->which << std::endl;
            UsePowerup(m_heldItem->which);
            m_heldItem.reset();
        }
    }

    if (IsKeyPressed(GameKeys::Pause))
    {
        m_isPaused = true;
    }

    if (IsKeyPressed(GameKeys::Exit))
    {
        m_quit = true;
    }
}

Vector2 PrairieKing::GetRandomVector2(float minX, float maxX, float minY, float maxY)
{
    return {GetRandomFloat(minX, maxX), GetRandomFloat(minY, maxY)};
}

float PrairieKing::GetRandomFloat(float min, float max)
{
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void PrairieKing::SpawnBullets(const std::vector<int> &directions, Vector2 spawn)
{
    if (directions.empty())
    {
        return;
    }

    // Calculate bullet starting position
    Vector2 bulletSpawn = {
        spawn.x + static_cast<float>(GetTileSize()) / 2.0f,
        spawn.y + static_cast<float>(GetTileSize()) / 2.0f};

    // Determine bullet count based on powerups
    int bulletCount = 1;
    if (m_activePowerups.find(POWERUP_SHOTGUN) != m_activePowerups.end())
    {
        bulletCount = 3;
    }

    // Handle diagonal shots
    if (directions.size() > 1)
    {
        // Calculate diagonal motion
        Vector2 diagonalMotion = {0, 0};

        // Combine the two directions for diagonal movement
        for (int dir : directions)
        {
            switch (dir)
            {
            case 0: // Up
                diagonalMotion.y -= BULLET_SPEED;
                break;
            case 1: // Right
                diagonalMotion.x += BULLET_SPEED;
                break;
            case 2: // Down
                diagonalMotion.y += BULLET_SPEED;
                break;
            case 3: // Left
                diagonalMotion.x -= BULLET_SPEED;
                break;
            }
        }

        // Normalize diagonal motion to maintain consistent speed
        float length = sqrtf(diagonalMotion.x * diagonalMotion.x + diagonalMotion.y * diagonalMotion.y);
        if (length > 0)
        {
            diagonalMotion.x = (diagonalMotion.x / length) * BULLET_SPEED;
            diagonalMotion.y = (diagonalMotion.y / length) * BULLET_SPEED;
        }

        // Spawn diagonal bullets
        if (m_activePowerups.find(POWERUP_SHOTGUN) != m_activePowerups.end())
        {
            // Shotgun spread pattern for diagonal shots
            float spreadAngle = 0.2f; // Spread angle in radians
            float baseAngle = atan2f(diagonalMotion.y, diagonalMotion.x);

            for (int i = 0; i < bulletCount; i++)
            {
                float angle = baseAngle;
                if (i == 1)
                    angle += spreadAngle;
                if (i == 2)
                    angle -= spreadAngle;

                Vector2 spreadMotion = {
                    cosf(angle) * BULLET_SPEED,
                    sinf(angle) * BULLET_SPEED};

                m_bullets.push_back(CowboyBullet(bulletSpawn, spreadMotion, m_bulletDamage));
            }
        }
        else
        {
            // Single diagonal bullet
            m_bullets.push_back(CowboyBullet(bulletSpawn, diagonalMotion, m_bulletDamage));
        }
    }
    // Handle single direction shots
    else
    {
        Vector2 bulletMotion = {0, 0};
        switch (directions[0])
        {
        case 0: // Up
            bulletMotion.y = -BULLET_SPEED;
            break;
        case 1: // Right
            bulletMotion.x = BULLET_SPEED;
            break;
        case 2: // Down
            bulletMotion.y = BULLET_SPEED;
            break;
        case 3: // Left
            bulletMotion.x = -BULLET_SPEED;
            break;
        }

        // Handle shotgun spread for single direction
        if (m_activePowerups.find(POWERUP_SHOTGUN) != m_activePowerups.end())
        {
            for (int i = 0; i < bulletCount; i++)
            {
                Vector2 spreadMotion = bulletMotion;
                if (bulletMotion.x == 0)
                { // Vertical shot
                    spreadMotion.x = (i == 0) ? 0.0f : (i == 1 ? -2.0f : 2.0f);
                }
                else
                { // Horizontal shot
                    spreadMotion.y = (i == 0) ? 0.0f : (i == 1 ? -2.0f : 2.0f);
                }
                m_bullets.push_back(CowboyBullet(bulletSpawn, spreadMotion, m_bulletDamage));
            }
        }
        else
        {
            m_bullets.push_back(CowboyBullet(bulletSpawn, bulletMotion, m_bulletDamage));
        }
    }

    // Play gunshot sound
    PlaySound(GetSound("Cowboy_gunshot"));
}

bool PrairieKing::IsSpawnQueueEmpty()
{
    for (int i = 0; i < 4; i++)
    {
        if (!m_spawnQueue[i].empty())
        {
            return false;
        }
    }
    return true;
}

bool PrairieKing::IsMapTilePassable(int tileType)
{
    if (tileType <= 1 || (tileType >= 5 && tileType <= 9))
    {
        return false;
    }
    return true;
}

bool PrairieKing::IsMapTilePassableForMonsters(int tileType)
{
    if (tileType == 5 || (tileType >= 7 && tileType <= 9))
    {
        return false;
    }
    return true;
}

bool PrairieKing::IsCollidingWithMonster(Rectangle r, CowboyMonster *subject)
{
    for (auto monster : m_monsters)
    {
        if (monster != subject && CheckCollisionRecs(r, monster->position))
        {
            return true;
        }
    }
    return false;
}

bool PrairieKing::IsCollidingWithMapForMonsters(Rectangle positionToCheck)
{
    int tileX1 = static_cast<int>(positionToCheck.x) / GetTileSize();
    int tileY1 = static_cast<int>(positionToCheck.y) / GetTileSize();
    int tileX2 = static_cast<int>(positionToCheck.x + positionToCheck.width - 1) / GetTileSize();
    int tileY2 = static_cast<int>(positionToCheck.y + positionToCheck.height - 1) / GetTileSize();

    for (int y = tileY1; y <= tileY2; y++)
    {
        for (int x = tileX1; x <= tileX2; x++)
        {
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
            {
                if (!IsMapTilePassableForMonsters(m_map[x][y]))
                {
                    return true;
                }
            }
            else
            {
                // Off-map is considered a collision
                return true;
            }
        }
    }
    return false;
}

bool PrairieKing::IsCollidingWithMap(Rectangle positionToCheck)
{
    int tileX1 = static_cast<int>(positionToCheck.x) / GetTileSize();
    int tileY1 = static_cast<int>(positionToCheck.y) / GetTileSize();
    int tileX2 = static_cast<int>(positionToCheck.x + positionToCheck.width - 1) / GetTileSize();
    int tileY2 = static_cast<int>(positionToCheck.y + positionToCheck.height - 1) / GetTileSize();

    for (int y = tileY1; y <= tileY2; y++)
    {
        for (int x = tileX1; x <= tileX2; x++)
        {
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
            {
                if (!IsMapTilePassable(m_map[x][y]))
                {
                    return true;
                }
            }
            else
            {
                // Off-map is considered a collision
                return true;
            }
        }
    }
    return false;
}

bool PrairieKing::IsCollidingWithMap(Vector2 position)
{
    int tileX = static_cast<int>(position.x) / GetTileSize();
    int tileY = static_cast<int>(position.y) / GetTileSize();

    if (tileX >= 0 && tileX < MAP_WIDTH && tileY >= 0 && tileY < MAP_HEIGHT)
    {
        return !IsMapTilePassable(m_map[tileX][tileY]);
    }
    return true; // Off the map is considered collision
}

void PrairieKing::AddPlayerMovementDirection(int direction)
{
    if (!m_gopherTrain)
    {
        // Buscar si la dirección ya existe
        auto it = std::find(m_playerMovementDirections.begin(), m_playerMovementDirections.end(), direction);

        if (it == m_playerMovementDirections.end())
        {
            // Si la dirección no existe, agregarla
            m_playerMovementDirections.push_back(direction);
        }
    }
}

void PrairieKing::AddPlayerShootingDirection(int direction)
{
    // Buscar si la dirección ya existe
    auto it = std::find(m_playerShootingDirections.begin(), m_playerShootingDirections.end(), direction);

    if (it == m_playerShootingDirections.end())
    {
        // Si la dirección no existe, agregarla
        m_playerShootingDirections.push_back(direction);
    }
}

void PrairieKing::StartShoppingLevel()
{
    // Reset merchant position and dimensions off screen
    m_merchantBox = {
        static_cast<float>(9 * GetTileSize() - GetTileSize()),  // Center the merchant
        static_cast<float>(-GetTileSize()),                     // Start above screen
        static_cast<float>(GetTileSize()),                      // Proper width
        static_cast<float>(GetTileSize())                       // Proper height
    };

    // Set shopping state flags
    m_shopping = true;
    m_merchantArriving = true;
    m_merchantLeaving = false;
    m_merchantShopOpen = false;

    // Stop music
    StopMusicStream(m_overworldSong);

    // Clear enemies
    for (auto monster : m_monsters)
    {
        delete monster;
    }
    m_monsters.clear();

    // Enable player movement down to next map
    m_waitingForPlayerToMoveDownAMap = true;

    // Clear previous store items
    m_storeItems.clear();

    // Add items based on wave
    if (m_whichWave == 2)
    {
        // Wave 2 shop items
        m_storeItems[Rectangle{static_cast<float>(7 * GetTileSize() + 12),
                               static_cast<float>(8 * GetTileSize() - GetTileSize() * 2),
                               static_cast<float>(GetTileSize()),
                               static_cast<float>(GetTileSize())}] = ITEM_RUNSPEED1;

        m_storeItems[Rectangle{static_cast<float>(8 * GetTileSize() + 24),
                               static_cast<float>(8 * GetTileSize() - GetTileSize() * 2),
                               static_cast<float>(GetTileSize()),
                               static_cast<float>(GetTileSize())}] = ITEM_FIRESPEED1;

        m_storeItems[Rectangle{static_cast<float>(9 * GetTileSize() + 36),
                               static_cast<float>(8 * GetTileSize() - GetTileSize() * 2),
                               static_cast<float>(GetTileSize()),
                               static_cast<float>(GetTileSize())}] = ITEM_AMMO1;
    }
    else
    {
        // Regular shop items based on player levels
        m_storeItems[Rectangle{static_cast<float>(7 * GetTileSize() + 12),
                               static_cast<float>(8 * GetTileSize() - GetTileSize() * 2),
                               static_cast<float>(GetTileSize()),
                               static_cast<float>(GetTileSize())}] =
            (m_runSpeedLevel >= 2) ? ITEM_LIFE : (ITEM_RUNSPEED1 + m_runSpeedLevel);

        m_storeItems[Rectangle{static_cast<float>(8 * GetTileSize() + 24),
                               static_cast<float>(8 * GetTileSize() - GetTileSize() * 2),
                               static_cast<float>(GetTileSize()),
                               static_cast<float>(GetTileSize())}] =
            (m_fireSpeedLevel < 3) ? ITEM_FIRESPEED1 + m_fireSpeedLevel : ((m_ammoLevel >= 3 && !m_spreadPistol) ? ITEM_SPREADPISTOL : ITEM_STAR);

        m_storeItems[Rectangle{static_cast<float>(9 * GetTileSize() + 36),
                               static_cast<float>(8 * GetTileSize() - GetTileSize() * 2),
                               static_cast<float>(GetTileSize()),
                               static_cast<float>(GetTileSize())}] =
            (m_ammoLevel < 3) ? (ITEM_AMMO1 + m_ammoLevel) : ITEM_STAR;
    }
    
    // Make sure we have the next map ready
    GetMap(m_whichWave, m_nextMap);
}

int PrairieKing::GetPriceForItem(int whichItem) const {
    switch (whichItem) {
        case ITEM_AMMO1: return 15;
        case ITEM_AMMO2: return 30;  
        case ITEM_AMMO3: return 45;
        case ITEM_FIRESPEED1: return 10;
        case ITEM_FIRESPEED2: return 20;
        case ITEM_FIRESPEED3: return 30;
        case ITEM_LIFE: return 10;
        case ITEM_RUNSPEED1: return 8;
        case ITEM_RUNSPEED2: return 20;
        case ITEM_SPREADPISTOL: return 99;
        case ITEM_STAR: return 10;
        default: return 5;
    }
}

void PrairieKing::GetMap(int wave, int (&newMap)[MAP_WIDTH][MAP_HEIGHT])
{
    // Initialize the map with default values
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            if ((x == 0 || x == 15 || y == 0 || y == 15) && 
                (x <= 6 || x >= 10) && (y <= 6 || y >= 10))
            {
                newMap[x][y] = MAP_CACTUS; // 5
            }
            else if (x == 0 || x == 15 || y == 0 || y == 15)
            {
                // Random barrier type for edges (0 = barrier1, 1 = barrier2)
                newMap[x][y] = (GetRandomFloat(0, 1) < 0.15f) ? MAP_BARRIER2 : MAP_BARRIER1;
            }
            else if (x == 1 || x == 14 || y == 1 || y == 14)
            {
                newMap[x][y] = MAP_ROCKY1; // 2
            }
            else
            {
                // Random terrain for interior (3 = desert, 4 = grassy)
                newMap[x][y] = (GetRandomFloat(0, 1) < 0.1f) ? MAP_GRASSY : MAP_DESERT;
            }
        }
    }

    // Customize map based on wave number
    switch (wave)
    {
    case -1: // End cutscene map
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                if (newMap[x][y] == MAP_BARRIER1 || newMap[x][y] == MAP_BARRIER2 || 
                    newMap[x][y] == MAP_ROCKY1 || newMap[x][y] == MAP_CACTUS)
                {
                    newMap[x][y] = MAP_DESERT; // 3
                }
            }
        }
        
        // Add special features for endgame map
        newMap[3][1] = MAP_CACTUS;
        newMap[8][2] = MAP_CACTUS;
        newMap[13][1] = MAP_CACTUS;
        newMap[5][0] = MAP_BARRIER1;
        newMap[10][2] = MAP_ROCKY1;
        newMap[15][2] = MAP_BARRIER2;
        newMap[14][12] = MAP_CACTUS;
        
        // Add fence walls
        newMap[10][6] = MAP_FENCE;
        newMap[11][6] = MAP_FENCE;
        newMap[12][6] = MAP_FENCE;
        newMap[13][6] = MAP_FENCE;
        newMap[14][6] = MAP_FENCE;
        newMap[14][7] = MAP_FENCE;
        newMap[14][8] = MAP_FENCE;
        newMap[14][9] = MAP_FENCE;
        newMap[14][10] = MAP_FENCE;
        newMap[14][11] = MAP_FENCE;
        newMap[14][12] = MAP_FENCE;
        newMap[14][13] = MAP_FENCE;
        
        // Add trenches
        for (int k = 0; k < 16; k++)
        {
            newMap[k][3] = ((k % 2 == 0) ? MAP_TRENCH1 : MAP_TRENCH2);
        }
        
        // Add bridge and other features
        newMap[3][3] = MAP_BRIDGE; // 10
        newMap[7][8] = MAP_ROCKY1;
        newMap[8][8] = MAP_ROCKY1;
        newMap[4][11] = MAP_ROCKY1;
        newMap[11][12] = MAP_ROCKY1;
        newMap[9][11] = MAP_ROCKY1;
        newMap[3][9] = MAP_ROCKY1;
        newMap[2][12] = MAP_CACTUS;
        newMap[8][13] = MAP_CACTUS;
        newMap[12][11] = MAP_CACTUS;
        newMap[7][14] = MAP_BARRIER1;
        newMap[6][14] = MAP_ROCKY1;
        newMap[8][14] = MAP_ROCKY1;
        newMap[7][13] = MAP_ROCKY1;
        newMap[7][15] = MAP_ROCKY1;
        break;

    case 0:
        //Default map
        break;

    case 1:
        // Add fence corners
        newMap[4][4] = MAP_FENCE;
        newMap[4][5] = MAP_FENCE;
        newMap[5][4] = MAP_FENCE;
        
        newMap[12][4] = MAP_FENCE;
        newMap[11][4] = MAP_FENCE;
        newMap[12][5] = MAP_FENCE;
        
        newMap[4][12] = MAP_FENCE;
        newMap[5][12] = MAP_FENCE;
        newMap[4][11] = MAP_FENCE;
        
        newMap[12][12] = MAP_FENCE;
        newMap[11][12] = MAP_FENCE;
        newMap[12][11] = MAP_FENCE;
        break;

    case 2:
        // Add central barriers and corner cacti
        newMap[8][5] = MAP_FENCE;
        newMap[11][8] = MAP_FENCE;
        newMap[8][11] = MAP_FENCE;
        newMap[5][8] = MAP_FENCE;
        
        //newMap[1][1] = MAP_CACTUS;
        //newMap[14][1] = MAP_CACTUS;
        //newMap[14][14] = MAP_CACTUS;
        //newMap[1][14] = MAP_CACTUS;
        //newMap[2][1] = MAP_CACTUS;
        //newMap[13][1] = MAP_CACTUS;
        //newMap[13][14] = MAP_CACTUS;
        //newMap[2][14] = MAP_CACTUS;
        //newMap[1][2] = MAP_CACTUS;
        //newMap[14][2] = MAP_CACTUS;
        //newMap[14][13] = MAP_CACTUS;
        //newMap[1][13] = MAP_CACTUS;
        break;

    case 3:
        // Fence perimeters in the middle

        newMap[5][5] = MAP_FENCE;
        newMap[6][5] = MAP_FENCE;
        newMap[7][5] = MAP_FENCE;
        newMap[9][5] = MAP_FENCE;
        newMap[10][5] = MAP_FENCE;
        newMap[11][5] = MAP_FENCE;
        
        newMap[5][11] = MAP_FENCE;
        newMap[6][11] = MAP_FENCE;
        newMap[7][11] = MAP_FENCE;
        newMap[9][11] = MAP_FENCE;
        newMap[10][11] = MAP_FENCE;
        newMap[11][11] = MAP_FENCE;
        
        newMap[5][6] = MAP_FENCE;
        newMap[5][7] = MAP_FENCE;
        newMap[5][9] = MAP_FENCE;
        newMap[5][10] = MAP_FENCE;
        
        newMap[11][6] = MAP_FENCE;
        newMap[11][7] = MAP_FENCE;
        newMap[11][9] = MAP_FENCE;
        newMap[11][10] = MAP_FENCE;
        break;
    case 4:
    case 8:
        {
            // Replace cacti with random barriers
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                for (int x = 0; x < MAP_WIDTH; x++)
                {
                    if (newMap[x][y] == MAP_CACTUS)
                    {
                        newMap[x][y] = (GetRandomFloat(0, 1) < 0.5f) ? MAP_BARRIER2 : MAP_BARRIER1;
                    }
                }
            }
            
            // Add trench through middle
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                newMap[x][8] = (GetRandomFloat(0, 1) < 0.5f) ? MAP_TRENCH1 : MAP_TRENCH2;
            }
            
            // Add fences and cacti
            newMap[8][4] = MAP_FENCE;
            newMap[8][12] = MAP_FENCE;
            newMap[9][12] = MAP_FENCE;
            newMap[7][12] = MAP_FENCE;
            newMap[5][6] = MAP_CACTUS;
            newMap[10][6] = MAP_CACTUS;
        }
        break;

    case 5:
        // Add lots of corner cacti
        newMap[1][1] = MAP_CACTUS;
        newMap[14][1] = MAP_CACTUS;
        newMap[14][14] = MAP_CACTUS;
        newMap[1][14] = MAP_CACTUS;
        newMap[2][1] = MAP_CACTUS;
        newMap[13][1] = MAP_CACTUS;
        newMap[13][14] = MAP_CACTUS;
        newMap[2][14] = MAP_CACTUS;
        newMap[1][2] = MAP_CACTUS;
        newMap[14][2] = MAP_CACTUS;
        newMap[14][13] = MAP_CACTUS;
        newMap[1][13] = MAP_CACTUS;
        
        // Add more cacti
        newMap[3][1] = MAP_CACTUS;
        newMap[13][1] = MAP_CACTUS;
        newMap[13][13] = MAP_CACTUS;
        newMap[1][13] = MAP_CACTUS;
        newMap[1][3] = MAP_CACTUS;
        newMap[13][3] = MAP_CACTUS;
        newMap[12][13] = MAP_CACTUS;
        newMap[3][14] = MAP_CACTUS;
        newMap[3][3] = MAP_CACTUS;
        newMap[13][12] = MAP_CACTUS;
        newMap[13][12] = MAP_CACTUS;
        newMap[3][12] = MAP_CACTUS;
        break;

    case 6:
        // Add rocks, cacti, and fences
        newMap[4][5] = MAP_ROCKY1;
        newMap[12][10] = MAP_CACTUS;
        newMap[10][9] = MAP_CACTUS;
        newMap[5][12] = MAP_ROCKY1;
        newMap[5][9] = MAP_CACTUS;
        newMap[12][12] = MAP_CACTUS;
        newMap[3][4] = MAP_CACTUS;
        newMap[2][3] = MAP_CACTUS;
        newMap[11][3] = MAP_CACTUS;
        newMap[10][6] = MAP_CACTUS;
        
        // Add fences
        newMap[5][9] = MAP_FENCE;
        newMap[10][12] = MAP_FENCE;
        newMap[3][12] = MAP_FENCE;
        newMap[10][8] = MAP_FENCE;
        break;

    case 7:
        // Add trenches through middle
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            newMap[x][5] = (x % 2 == 0) ? MAP_TRENCH1 : MAP_TRENCH2;
            newMap[x][10] = (x % 2 == 0) ? MAP_TRENCH1 : MAP_TRENCH2;
        }
        
        // Add bridges
        newMap[4][5] = MAP_BRIDGE;
        newMap[8][5] = MAP_BRIDGE;
        newMap[12][5] = MAP_BRIDGE;
        newMap[4][10] = MAP_BRIDGE;
        newMap[8][10] = MAP_BRIDGE;
        newMap[12][10] = MAP_BRIDGE;
        break;

    case 9:
        // Add cacti squares
        newMap[4][4] = MAP_CACTUS;
        newMap[5][4] = MAP_CACTUS;
        newMap[10][4] = MAP_CACTUS;
        newMap[12][4] = MAP_CACTUS;
        newMap[4][5] = MAP_CACTUS;
        newMap[5][5] = MAP_CACTUS;
        newMap[10][5] = MAP_CACTUS;
        newMap[12][5] = MAP_CACTUS;
        
        newMap[4][10] = MAP_CACTUS;
        newMap[5][10] = MAP_CACTUS;
        newMap[10][10] = MAP_CACTUS;
        newMap[12][10] = MAP_CACTUS;
        newMap[4][12] = MAP_CACTUS;
        newMap[5][12] = MAP_CACTUS;
        newMap[10][12] = MAP_CACTUS;
        newMap[12][12] = MAP_CACTUS;
        break;

    case 10:
        // Add trenches at top and bottom
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            newMap[x][1] = (x % 2 == 0) ? MAP_TRENCH1 : MAP_TRENCH2;
            newMap[x][14] = (x % 2 == 0) ? MAP_TRENCH1 : MAP_TRENCH2;
        }
        
        // Add bridges and cacti
        newMap[8][1] = MAP_BRIDGE;
        newMap[7][1] = MAP_BRIDGE;
        newMap[9][1] = MAP_BRIDGE;
        newMap[8][14] = MAP_BRIDGE;
        newMap[7][14] = MAP_BRIDGE;
        newMap[9][14] = MAP_BRIDGE;
        
        newMap[6][8] = MAP_CACTUS;
        newMap[10][8] = MAP_CACTUS;
        newMap[8][6] = MAP_CACTUS;
        newMap[8][9] = MAP_CACTUS;
        break;

    case 11:
        // Add fence borders
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            newMap[x][0] = MAP_FENCE;
            newMap[x][15] = MAP_FENCE;
            if (x % 2 == 0)
            {
                newMap[x][1] = MAP_CACTUS;
                newMap[x][14] = MAP_CACTUS;
            }
        }
        break;

    case 12:
        {
            // Convert barriers to cactus
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                for (int x = 0; x < MAP_WIDTH; x++)
                {
                    if (newMap[x][y] == MAP_BARRIER1 || newMap[x][y] == MAP_BARRIER2)
                    {
                        newMap[x][y] = MAP_CACTUS;
                    }
                }
            }
            
            // Add trench borders
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                newMap[x][0] = (x % 2 == 0) ? MAP_TRENCH1 : MAP_TRENCH2;
                newMap[x][15] = (x % 2 == 0) ? MAP_TRENCH1 : MAP_TRENCH2;
            }
            
            // Add bridge border
            Rectangle r = {1, 1, 14, 14};
            std::vector<Vector2> border = GetBorderPoints(r);
            for (const auto &pt : border)
            {
                newMap[static_cast<int>(pt.x)][static_cast<int>(pt.y)] = MAP_BRIDGE;
            }
            
            // Add rocky inner border
            r = {2, 2, 12, 12};
            border = GetBorderPoints(r);
            for (const auto &pt : border)
            {
                newMap[static_cast<int>(pt.x)][static_cast<int>(pt.y)] = MAP_ROCKY1;
            }
            return; // Important: return early for wave 12
        }
        break;

    default:
        // Default map additions
        newMap[4][4] = MAP_CACTUS;
        newMap[12][4] = MAP_CACTUS;
        newMap[4][12] = MAP_CACTUS;
        newMap[12][12] = MAP_CACTUS;
        break;
    }
}

std::vector<Vector2> PrairieKing::GetBorderPoints(const Rectangle &rect)
{
    std::vector<Vector2> points;

    // Add top and bottom borders
    for (int x = rect.x; x < rect.x + rect.width; x++)
    {
        points.push_back({static_cast<float>(x), static_cast<float>(rect.y)});                   // Top
        points.push_back({static_cast<float>(x), static_cast<float>(rect.y + rect.height - 1)}); // Bottom
    }

    // Add left and right borders (excluding corners already added)
    for (int y = rect.y + 1; y < rect.y + rect.height - 1; y++)
    {
        points.push_back({static_cast<float>(rect.x), static_cast<float>(y)});                  // Left
        points.push_back({static_cast<float>(rect.x + rect.width - 1), static_cast<float>(y)}); // Right
    }

    return points;
}

void PrairieKing::SetButtonState(GameKeys key, bool pressed)
{
    if (key == GameKeys::Pause)
    {
        m_isPaused = !m_isPaused;
        std::cout << "Pause screen: " << (m_isPaused ? "ON" : "OFF") << std::endl;
        return;
    }
    // Special handling for debug keys
    if (key >= GameKeys::DebugToggle && key <= GameKeys::DebugClearMonsters)
    {
        if (pressed && !IsKeyDown(key))
        { // Only trigger on initial press
            // Handle debug toggle separately
            if (key == GameKeys::DebugToggle)
            {
                m_debugMode = !m_debugMode;
                std::cout << "Debug mode: " << (m_debugMode ? "ON" : "OFF") << std::endl;
                return;
            }

            // Only process other debug commands if debug mode is on
            if (m_debugMode)
            {
                switch (key)
                {
                case GameKeys::DebugSpawn1:
                    SpawnDebugMonster(ORC);
                    break;
                case GameKeys::DebugSpawn2:
                    SpawnDebugMonster(EVIL_BUTTERFLY);
                    break;
                case GameKeys::DebugSpawn3:
                    SpawnDebugMonster(OGRE);
                    break;
                case GameKeys::DebugSpawn4:
                    SpawnDebugMonster(MUMMY);
                    break;
                case GameKeys::DebugSpawn5:
                    SpawnDebugMonster(IMP);
                    break;
                case GameKeys::DebugSpawn6:
                    SpawnDebugMonster(MUSHROOM);
                    break;
                case GameKeys::DebugSpawn7:
                    SpawnDebugMonster(SPIKEY);
                    break;
                case GameKeys::DebugSpawn8:
                    SpawnDebugPowerup(GetRandomInt(0, 5));
                    break;
                case GameKeys::DebugSpawn9:
                    SpawnDebugPowerup(GetRandomInt(6, 10));
                    break;
                case GameKeys::DebugAddLife:
                    m_lives++;
                    std::cout << "Added life. Total: " << m_lives << std::endl;
                    break;
                case GameKeys::DebugAddCoins:
                    m_coins += 10;
                    std::cout << "Added 10 coins. Total: " << m_coins << std::endl;
                    break;
                case GameKeys::DebugIncDamage:
                    m_bulletDamage++;
                    std::cout << "Increased damage to: " << m_bulletDamage << std::endl;
                    break;
                case GameKeys::DebugClearMonsters:
                {
                    int count = m_monsters.size();
                    for (auto monster : m_monsters)
                        delete monster;
                    m_monsters.clear();
                    std::cout << "Cleared " << count << " monsters" << std::endl;
                }
                break;
                default:
                    break;
                }
            }
        }
        return;
    }

    // Normal key handling
    if (pressed)
    {
        m_buttonHeldState.insert(key);
        m_buttonHeldFrames[key] = 1;
    }
    else
    {
        m_buttonHeldState.erase(key);
        m_buttonHeldFrames.erase(key);
    }
}

void PrairieKing::Update(float deltaTime)
{
    if (!m_isPaused)
    {
        m_waveTimer -= deltaTime * 1000.0f;
    }

    // Update shopping timer for merchant animation
    if (m_merchantArriving || m_merchantLeaving) {
        m_shoppingTimer += deltaTime * 1000.0f;
    }

    UpdateMusicStream(m_overworldSong);

    // Update button held state
    for (const auto &key : m_buttonHeldState)
    {
        m_buttonHeldFrames[key]++;
    }

    // Process player inputs first
    ProcessInputs();

    if (m_quit)
        return;

    if (m_isPaused)
    {
        PauseScreen();
        return;
    }

    // Handle game over state
    if (m_gameOver)
    {
        DiscordManager::UpdatePresence("Game Over", "Press Enter to retry");
        return;
    }

    if (m_gameRestartTimer > 0)
    {
        m_gameRestartTimer -= static_cast<int>(deltaTime * 1000.0f);
        if (m_gameRestartTimer <= 0)
        {
            // Full reset for new game
            if (m_whichRound == 0 || !m_endCutscene)
            {
                Initialize();
            }
            else
            {
                // Preserve progress for New Game+
                Initialize();
                m_coins = m_progress.coins;
                m_ammoLevel = m_progress.ammoLevel;
                m_bulletDamage = m_progress.bulletDamage;
                m_fireSpeedLevel = m_progress.fireSpeedLevel;
                m_runSpeedLevel = m_progress.runSpeedLevel;
                m_lives = m_progress.lives;
                m_spreadPistol = m_progress.spreadPistol;
                m_whichRound = m_progress.whichRound;
            }
        }
        return;
    }

    // Update timers
    if (m_deathTimer > 0)
    {
        m_deathTimer -= deltaTime * 1000.0f;
    }

    if (m_playerInvincibleTimer > 0)
    {
        m_playerInvincibleTimer -= static_cast<int>(deltaTime * 1000.0f);

        // When invincibility timer reaches 0, resume the music
        if (m_playerInvincibleTimer <= 0)
        {
            ResumeMusicStream(m_overworldSong);
        }
    }

    // Update cactus dance timer
    m_cactusDanceTimer += deltaTime * 1000.0f;
    if (m_cactusDanceTimer >= 1600.0f) // Full dance cycle is 1.6 seconds
    {
        m_cactusDanceTimer = 0.0f;
    }

    if (m_motionPause > 0)
    {
        m_motionPause -= deltaTime * 1000.0f;
        // Behavior after pause would be handled by callbacks if needed
    }

    if (m_zombieModeTimer > 0.0f)
    {
        m_zombieModeTimer -= deltaTime * 1000.0f;
    }

    if (m_holdItemTimer > 0.0f)
    {
        m_holdItemTimer -= deltaTime * 1000.0f;
        return;
    }

    // Update game elements that should always run
    UpdateBullets(deltaTime);
    UpdatePlayer(deltaTime);

    // Update temporary sprites
    for (int i = m_temporarySprites.size() - 1; i >= 0; i--)
    {
        if (m_temporarySprites[i].Update(deltaTime))
        {
            m_temporarySprites.erase(m_temporarySprites.begin() + i);
        }
    }

    // Update powerups and their timers
    for (int i = m_powerups.size() - 1; i >= 0; i--)
    {
        // Move powerups toward player if close enough
        if (CheckCollisionRecs(m_playerBoundingBox,
                               Rectangle{m_powerups[i].position.x, m_powerups[i].position.y,
                                         static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}))
        {
            // Movement logic for powerup attraction
            Vector2 direction = Vector2Subtract(m_playerPosition, m_powerups[i].position);
            float length = Vector2Length(direction);
            if (length > 0)
            {
                direction = Vector2Scale(direction, 1.0f / length);
                m_powerups[i].position = Vector2Add(m_powerups[i].position,
                                                    Vector2Scale(direction, 5.0f * deltaTime));
            }
        }

        // Update duration
        m_powerups[i].duration -= deltaTime * 1000.0f;
        if (m_powerups[i].duration <= 0)
        {
            m_powerups.erase(m_powerups.begin() + i);
        }
    }

    // Update active powerup timers
    for (auto it = m_activePowerups.begin(); it != m_activePowerups.end();)
    {
        it->second -= deltaTime * 1000.0f;
        if (it->second <= 0)
        {
            it = m_activePowerups.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Handle between wave timer
    if (m_betweenWaveTimer > 0)
    {
        m_betweenWaveTimer -= deltaTime * 1000.0f;
        if (m_betweenWaveTimer <= 0)
        {
            // Remove the death check to allow wave to start after respawning
            m_waveTimer = GameConstants::WAVE_DURATION;
            m_waveCompleted = false;
            UpdateMonsterChancesForWave();
            if (!IsMusicStreamPlaying(m_overworldSong))
            {
                PlayMusicStream(m_overworldSong);
            }
        }
        return;
    }

    // Handle wave state
    if (!m_waveCompleted)
    {

        if (m_waveTimer > 0 && !m_shootoutLevel)
        {
            m_waveTimer -= deltaTime * 1000.0f;

            // Only spawn monsters if not in special states and player is alive
            if (!m_scrollingMap && !m_merchantArriving && !m_merchantLeaving &&
                !m_waitingForPlayerToMoveDownAMap && m_deathTimer <= 0)
            {
                float spawnChance = 0.03f;
                if (m_monsters.empty())
                {
                    spawnChance = 0.1f;
                }

                if (GetRandomFloat(0.0f, 1.0f) < spawnChance)
                {
                    int numMonsters = 2;
                    if (m_whichWave > 5 && GetRandomFloat(0.0f, 1.0f) < 0.3f)
                    {
                        numMonsters = 4;
                    }

                    for (int i = 0; i < numMonsters; i++)
                    {
                        Vector2 spawnPoint = GetRandomSpawnPosition();
                        if (spawnPoint.x >= 0 && spawnPoint.y >= 0)
                        {
                            int monsterType = ChooseMonsterType(GetMonsterChancesForWave(m_whichWave));
                            CowboyMonster *monster = new CowboyMonster(m_assets, monsterType, spawnPoint);
                            AddMonster(monster);
                        }
                    }
                }
            }
        }

        // Check if wave should be completed
        if (m_waveTimer <= 0 && !m_waveCompleted)
        {
            if (m_monsters.empty() && IsSpawnQueueEmpty())
            {
                m_waveCompleted = true;
                m_whichWave++;
                m_died = false; // Reset death state when completing wave

                // Set up the next map/shop transition
                if (m_whichWave > 0)
                {
                    if (m_whichWave % 2 == 0)
                    {
                        StartShoppingLevel();
                    }
                    else
                    {
                        m_waitingForPlayerToMoveDownAMap = true;
                        m_map[8][15] = MAP_DESERT;
                        m_map[7][15] = MAP_DESERT;
                        m_map[9][15] = MAP_DESERT;
                        GetMap(m_whichWave, m_nextMap);
                    }
                }
            }
        }
    }

    // Add this check to your monster update in the Update method
    if (m_waveTimer <= 0 && !m_monsters.empty() && IsSpawnQueueEmpty())
    {
        bool onlySpikeys = true;
        for (const auto& monster : m_monsters)
        {
            if (monster->type != GameConstants::SPIKEY)
            {
                onlySpikeys = false;
                break;
            }
        }
        
        // If only Spikeys are left, make them easier to kill
        if (onlySpikeys)
        {
            for (auto& monster : m_monsters)
            {
                monster->health = 1;
            }
        }
    }

    if (m_shopping)
    {
        // Handle merchant animation
        if (m_merchantBox.y < 8 * GetTileSize() - GetTileSize() * 3 && m_merchantArriving)
        {
            m_merchantBox.y += 2;
            if (m_merchantBox.y >= 8 * GetTileSize() - GetTileSize() * 3)
            {
                m_merchantShopOpen = true;
                PlaySound(GetSound("cowboy_monsterhit"));

                // Clear path tiles
                m_map[8][15] = MAP_DESERT;
                m_map[7][15] = MAP_DESERT;
                m_map[8][14] = MAP_DESERT;
                m_map[7][14] = MAP_DESERT;

                // Set no pickup area around merchant
                m_shoppingCarpetNoPickup = Rectangle{
                    m_merchantBox.x,
                    m_merchantBox.y + GetTileSize(),
                    static_cast<float>(GetTileSize()),
                    static_cast<float>(GetTileSize() * 2) };
            }
        }

        // Handle purchases
        if (m_merchantShopOpen)
        {
            for (auto it = m_storeItems.begin(); it != m_storeItems.end();)
            {
                if (CheckCollisionRecs(m_playerBoundingBox, it->first) &&
                    m_coins >= GetPriceForItem(it->second))
                {

                    PlaySound(GetSound("Cowboy_Secret"));
                    m_holdItemTimer = 2500;
                    m_motionPause = 2500;
                    m_itemToHold = it->second;

                    // Remove purchased item
                    auto toErase = it;
                    ++it;
                    m_storeItems.erase(toErase);

                    m_merchantLeaving = true;
                    m_merchantArriving = false;
                    m_merchantShopOpen = false;
                    m_coins -= GetPriceForItem(m_itemToHold);

                    // Apply upgrades
                    ApplyPurchasedUpgrade(m_itemToHold);
                }
                else
                {
                    ++it;
                }
            }
        }

        // Handle merchant departure
        if (m_merchantLeaving)
        {
            m_merchantBox.y -= 2;
            if (m_merchantBox.y <= -GetTileSize())
            {
                m_shopping = false;
                m_merchantLeaving = false;
                m_merchantArriving = true;
                
                // Make sure to load the next wave map here
                //m_whichWave++; // Increment wave since we're moving to the next one
                GetMap(m_whichWave, m_nextMap); // Load the next map
                
                // Set up for map transition
                m_waitingForPlayerToMoveDownAMap = true;
                m_map[8][15] = MAP_DESERT; // Clear path at the bottom
                m_map[7][15] = MAP_DESERT;
                m_map[9][15] = MAP_DESERT;
            }
        }
    }

    // Update game elements
    for (auto monster : m_monsters)
    {
        monster->Move(m_playerPosition, deltaTime);
    }

    // Handle map scrolling
    if (m_scrollingMap)
    {
        m_newMapPosition -= GetTileSize() / 8;
        m_playerPosition.y -= GetTileSize() / 8;
        m_playerPosition.y += 3.0f;

        m_playerBoundingBox.x = m_playerPosition.x + GetTileSize() / 4;
        m_playerBoundingBox.y = m_playerPosition.y + GetTileSize() / 4;
        m_playerBoundingBox.width = GetTileSize() / 2;
        m_playerBoundingBox.height = GetTileSize() / 2;

        if (m_playerMovementDirections.empty())
        {
            m_playerMovementDirections.push_back(2);
        }

        m_playerMotionAnimationTimer += deltaTime * 1000.0f;
        m_playerMotionAnimationTimer = fmodf(m_playerMotionAnimationTimer, 400.0f);

        if (m_newMapPosition <= 0)
        {
            m_scrollingMap = false;
            memcpy(m_map, m_nextMap, sizeof(m_map));
            m_newMapPosition = 16 * GetTileSize();
            m_shopping = false;
            m_betweenWaveTimer = GameConstants::BETWEEN_WAVE_DURATION;
            m_waitingForPlayerToMoveDownAMap = false;
            m_playerMovementDirections.clear();
            ApplyLevelSpecificStates();
        }
    }

    // Update Discord Rich Presence based on game state
    if (m_shopping)
    {
        DiscordManager::UpdatePresence("Shopping", "Upgrading equipment");
    }
    else
    {
        const char *details = TextFormat("Wave %d", m_whichWave + 1);
        const char *state = TextFormat("Score: %d", m_score);
        DiscordManager::UpdatePresence(state, details);
    }
}

void PrairieKing::UpdateMonsterChancesForWave()
{
    switch (m_whichWave)
    {
    case 1:
    case 2:
    case 3:
        // Waves 1-3: Increase Orcs, Ogres, Spikey
        m_monsterChances[0] = Vector2Add(m_monsterChances[0], Vector2{0.001f, 0.02f}); // Orcs
        if (m_whichWave > 1)
        {
            m_monsterChances[2] = Vector2Add(m_monsterChances[2], Vector2{0.001f,0.01f});
        }
        break;

    case 4:
    case 5:
    case 6:
    case 7:
        // Waves 4-7: Remove Orcs/Spikey, add Mushrooms, increase Ghosts
        if (Vector2Equals(m_monsterChances[5], Vector2Zero()))
        {
            m_monsterChances[5] = {0.01f, 0.15f}; // Initialize Mushrooms
            if (m_whichRound > 0)
            {
                m_monsterChances[5] = {0.01f + static_cast<float>(m_whichRound) * 0.004f,
                                      0.15f + static_cast<float>(m_whichRound) * 0.04f};
            }
        }
        m_monsterChances[0] = Vector2Zero(); // Disable Orcs
        m_monsterChances[6] = Vector2Zero(); // Disable Spikey
        m_monsterChances[2] = Vector2Add(m_monsterChances[2], Vector2{0.002f, 0.02f}); // More Ogres
        m_monsterChances[5] = Vector2Add(m_monsterChances[5], Vector2{0.001f, 0.02f}); // More Mushrooms
        m_monsterChances[1] = Vector2Add(m_monsterChances[1], Vector2{0.0018f, 0.08f}); // More Ghosts
        if (m_whichRound > 0)
        {
            m_monsterChances[4] = {0.001f, 0.1f}; // Devils in New Game Plus
        }
        break;

    case 8:
    case 9:
    case 10:
    case 11:
        // Waves 8-11: Disable earlier monsters, add Mummies and Devils
        m_monsterChances[5] = Vector2Zero(); // Disable Mushrooms
        m_monsterChances[1] = Vector2Zero(); // Disable Ghosts
        m_monsterChances[2] = Vector2Zero(); // Disable Ogres
        
        if (Vector2Equals(m_monsterChances[3], Vector2Zero()))
        {
            m_monsterChances[3] = {0.012f, 0.4f}; // Initialize Mummies
            if (m_whichRound > 0)
            {
                m_monsterChances[3] = {0.012f + static_cast<float>(m_whichRound) * 0.005f,
                                      0.4f + static_cast<float>(m_whichRound) * 0.075f};
            }
        }
        
        if (Vector2Equals(m_monsterChances[4], Vector2Zero()))
        {
            m_monsterChances[4] = {0.003f, 0.1f}; // Initialize Devils
        }
        
        m_monsterChances[3] = Vector2Add(m_monsterChances[3], Vector2{0.002f, 0.05f}); // More Mummies
        m_monsterChances[4] = Vector2Add(m_monsterChances[4], Vector2{0.0015f, 0.04f}); // More Devils
        
        if (m_whichWave == 11)
        {
            m_monsterChances[4] = Vector2Add(m_monsterChances[4], Vector2{0.01f, 0.04f}); // Even more Devils
            m_monsterChances[3] = Vector2{m_monsterChances[3].x - 0.01f, m_monsterChances[3].y + 0.04f}; // Less Mummies, increase weight
        }
        break;
    }
}

void PrairieKing::Draw()
{
    if (m_gameOver || m_gameRestartTimer > 0)
    {
        // Draw black background
        DrawRectangle(
            static_cast<int>(m_topLeftScreenCoordinate.x),
            static_cast<int>(m_topLeftScreenCoordinate.y),
            16 * GetTileSize(),
            16 * GetTileSize(),
            BLACK);

        // Draw "GAME OVER" text and shadows
        const char *gameOverText = "Game Over";
        Vector2 textPos = {
            m_topLeftScreenCoordinate.x + 6.0f * GetTileSize(),
            m_topLeftScreenCoordinate.y + 7.0f * GetTileSize()};

        // Draw shadows
        DrawTextEx(m_assets.GetFont("title"), gameOverText,
                   Vector2{textPos.x - 1, textPos.y}, 48, 1, WHITE);
        DrawTextEx(m_assets.GetFont("title"), gameOverText,
                   Vector2{textPos.x + 1, textPos.y}, 48, 1, WHITE);
        DrawTextEx(m_assets.GetFont("title"), gameOverText,
                   textPos, 48, 1, WHITE);

        // Draw options
        const char *retryText = TextFormat("%sRetry", m_gameOverOption == 0 ? "> " : "  ");
        const char *quitText = TextFormat("%sQuit", m_gameOverOption == 1 ? "> " : "  ");

        // Only draw retry if not restarting or during blink
        if (m_gameRestartTimer <= 0 || m_gameRestartTimer / 500 % 2 == 0)
        {
            DrawTextEx(m_assets.GetFont("text"), retryText,
                       Vector2{m_topLeftScreenCoordinate.x + 6.0f * GetTileSize(),
                               m_topLeftScreenCoordinate.y + 9.0f * GetTileSize()},
                       32, 1, WHITE);
        }

        DrawTextEx(m_assets.GetFont("text"), quitText,
                   Vector2{m_topLeftScreenCoordinate.x + 6.0f * GetTileSize(),
                           m_topLeftScreenCoordinate.y + 9.0f * GetTileSize() + GetTileSize() * 2 / 3.0f},
                   32, 1, WHITE);

        return;
    }

    // 1. Background Layer (layerDepth: 0.0)
    // Draw the current game map
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            DrawTexturePro(
                GetTexture("cursors"),
                Rectangle{336.0f + 16.0f * m_map[x][y] + ((m_map[x][y] == 5 && m_cactusDanceTimer > 800.0f) ? 16.0f : 0.0f),
                           32.0f - m_world * 16.0f,
                           16.0f,
                           16.0f},
                Rectangle{m_topLeftScreenCoordinate.x + x * GetTileSize(),
                          m_topLeftScreenCoordinate.y + y * GetTileSize() + (m_scrollingMap ? (m_newMapPosition - 16 * GetTileSize()) : 0),
                          static_cast<float>(GetTileSize()),
                          static_cast<float>(GetTileSize())},
                Vector2{0, 0},
                0.0f,
                WHITE);
        }
    }

    // Draw scrolling map if needed
    if (m_scrollingMap)
    {
        // Draw next map coming in from below - using the buffered next map
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                DrawTexturePro(
                    GetTexture("cursors"),
                    Rectangle{336.0f + 16.0f * m_nextMap[x][y] + ((m_nextMap[x][y] == 5 && m_cactusDanceTimer > 800.0f) ? 16.0f : 0.0f),
                              32.0f - m_world * 16.0f,
                              16.0f,
                              16.0f},
                    Rectangle{m_topLeftScreenCoordinate.x + x * GetTileSize(),
                              m_topLeftScreenCoordinate.y + y * GetTileSize() + m_newMapPosition,
                              static_cast<float>(GetTileSize()),
                              static_cast<float>(GetTileSize())},
                    Vector2{0, 0},
                    0.0f,
                    WHITE);
            }
        }

        // Draw black borders above and below the visible map area
        DrawRectangle(
            static_cast<int>(m_topLeftScreenCoordinate.x),
            -1,
            16 * GetTileSize(),
            static_cast<int>(m_topLeftScreenCoordinate.y),
            BLACK);

        DrawRectangle(
            static_cast<int>(m_topLeftScreenCoordinate.x),
            static_cast<int>(m_topLeftScreenCoordinate.y + 16 * GetTileSize()),
            16 * GetTileSize(),
            static_cast<int>(m_topLeftScreenCoordinate.y + 2),
            BLACK);
    }

    // 2. Main Game Elements (layerDepth: 0.001 - 0.9)
    // Draw temporary sprites
    for (auto &sprite : m_temporarySprites)
    {
        sprite.Draw(GetTexture("cursors"));
    }

    // Draw powerups
    for (const auto &powerup : m_powerups)
    {
        powerup.Draw(GetTexture("cursors"), m_topLeftScreenCoordinate);
    }

    // Draw bullets (layerDepth: 0.9)
    for (const auto &bullet : m_bullets)
    {
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{390.0f, 112.0f + (m_bulletDamage - 1) * 4.0f, 4.0f, 4.0f},
            Rectangle{m_topLeftScreenCoordinate.x + bullet.position.x,
                      m_topLeftScreenCoordinate.y + bullet.position.y,
                      12.0f, 12.0f},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }

    // Draw enemy bullets (layerDepth: 0.9)
    for (const auto &bullet : m_enemyBullets)
    {
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{395.0f, 112.0f, 5.0f, 5.0f},
            Rectangle{m_topLeftScreenCoordinate.x + bullet.position.x,
                      m_topLeftScreenCoordinate.y + bullet.position.y,
                      15.0f, 15.0f},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }

    if (m_shopping) {
        DrawShopping(GetTexture("cursors"), m_topLeftScreenCoordinate);
    }

    // 3. Characters and Monsters (layerDepth: ~0.001 - 0.002 + position.Y/10000f)
    // Draw player
    if (m_deathTimer <= 0.0f && (m_playerInvincibleTimer <= 0 || m_playerInvincibleTimer / 100 % 2 == 0))
    {
        if (m_holdItemTimer > 0)
        {
            // Draw player holding item
            DrawTexturePro(
                GetTexture("cursors"),
                Rectangle{256.0f, 112.0f, 16.0f, 16.0f},
                Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                          m_topLeftScreenCoordinate.y + m_playerPosition.y,
                          48.0f, 48.0f},
                Vector2{0, 0},
                0.0f,
                WHITE);

            // Draw held item above player
            DrawTexturePro(
                GetTexture("cursors"),
                Rectangle{192.0f + m_itemToHold * 16.0f, 128.0f, 16.0f, 16.0f},
                Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                          m_topLeftScreenCoordinate.y + m_playerPosition.y - GetTileSize() / 2,
                          48.0f, 48.0f},
                Vector2{0, 0},
                0.0f,
                WHITE);
        }
        else if (m_zombieModeTimer > 8200)
        {
            // Draw flashing player sprite during transformation
            DrawTexturePro(
                GetTexture("cursors"),
                Rectangle{256.0f + ((static_cast<int>(m_zombieModeTimer / 200) % 2 == 0) ? 16.0f : 0.0f),
                          112.0f, 16.0f, 16.0f},
                Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                          m_topLeftScreenCoordinate.y + m_playerPosition.y,
                          48.0f, 48.0f},
                Vector2{0, 0},
                0.0f,
                WHITE);

            // Draw rising effect
            for (float y = m_playerPosition.y - GetTileSize(); y > -GetTileSize(); y -= GetTileSize())
            {
                DrawTexturePro(
                    GetTexture("cursors"),
                    Rectangle{240.0f + ((static_cast<int>(y / GetTileSize()) % 3 == 0) ? 16.0f : 0.0f),
                              96.0f, 16.0f, 16.0f},
                    Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                              m_topLeftScreenCoordinate.y + y,
                              48.0f, 48.0f},
                    Vector2{0, 0},
                    0.0f,
                    WHITE);
            }
            return;
        }
        else if (m_zombieModeTimer > 0)
        {
            // Draw zombie player during active mode
            DrawTexturePro(
                GetTexture("cursors"),
                Rectangle{224.0f + ((static_cast<int>(m_zombieModeTimer / 50) % 2 == 0) ? 16.0f : 0.0f),
                          112.0f, 16.0f, 16.0f},
                Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                          m_topLeftScreenCoordinate.y + m_playerPosition.y - GetTileSize() / 4,
                          48.0f, 48.0f},
                Vector2{0, 0},
                0.0f,
                WHITE);
        }
        else if (m_playerMovementDirections.empty() && m_playerShootingDirections.empty())
        {
            // Draw idle player
            DrawTexturePro(
                GetTexture("cursors"),
                Rectangle{368.0f, 112.0f, 16.0f, 16.0f},
                Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                          m_topLeftScreenCoordinate.y + m_playerPosition.y,
                          48.0f, 48.0f},
                Vector2{0, 0},
                0.0f,
                WHITE);
        }
        else
        {
            // Get facing direction from movement or shooting
            int facingDirection = m_playerShootingDirections.empty() ? (!m_playerMovementDirections.empty() ? m_playerMovementDirections[0] : 0) : m_playerShootingDirections.back();

            // Draw player feet - different animation when shooting
            int footFrame;
            if (!m_playerShootingDirections.empty() && m_playerMovementDirections.empty())
            {
                // Solo usa el frame fijo cuando dispara y NO está caminando
                footFrame = 0;
            }
            else if (!m_playerMovementDirections.empty())
            {
                // Usa la animación de caminar si se está moviendo
                footFrame = static_cast<int>(m_playerMotionAnimationTimer / 100.0f) % 4;
            }
            else
            {
                // Frame por defecto cuando está quieto
                footFrame = 0;
            }

            DrawTexturePro(
                GetTexture("cursors"),
                Rectangle{355.0f, 112.0f + footFrame * 3.0f, 10.0f, 3.0f},
                Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x + 9.0f,
                          m_topLeftScreenCoordinate.y + m_playerPosition.y + 39.0f,
                          30.0f, 9.0f},
                Vector2{0, 0},
                0.0f,
                WHITE);

            // Draw player body
            DrawTexturePro(
                GetTexture("cursors"),
                Rectangle{336.0f + facingDirection * 16.0f, 96.0f, 16.0f, 16.0f},
                Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                          m_topLeftScreenCoordinate.y + m_playerPosition.y,
                          48.0f, 48.0f},
                Vector2{0, 0},
                0.0f,
                WHITE);
        }
    }

    // Draw zombie mode effects
    if (m_zombieModeTimer > 8200)
    {
        // Draw zombie player sprite
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{256.0f + ((static_cast<int>(m_zombieModeTimer) / 200 % 2 == 0) ? 16.0f : 0.0f), 112.0f, 16.0f, 16.0f},
            Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                      m_topLeftScreenCoordinate.y + m_playerPosition.y,
                      48.0f, 48.0f},
            Vector2{0, 0},
            0.0f,
            WHITE);

        // Draw rising effects
        for (float y = m_playerPosition.y - GetTileSize(); y > -GetTileSize(); y -= GetTileSize())
        {
            DrawTexturePro(
                GetTexture("cursors"),
                Rectangle{240.0f + ((static_cast<int>(y / GetTileSize()) % 3 == 0) ? 16.0f : 0.0f),
                          64.0f, 16.0f, 16.0f},
                Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                          m_topLeftScreenCoordinate.y + y,
                          48.0f, 48.0f},
                Vector2{0, 0},
                0.0f,
                WHITE);
        }
    }

    // Draw monsters
    for (auto monster : m_monsters)
    {
        monster->Draw(GetTexture("cursors"), m_topLeftScreenCoordinate);
    }

    if (m_isPaused)
    {
        // Draw semi-transparent black background
        Color semiTransparentBlack = {0, 0, 0, 128}; // Last value (128) is alpha: 0-255
        DrawRectangle(
            static_cast<int>(m_topLeftScreenCoordinate.x),
            static_cast<int>(m_topLeftScreenCoordinate.y),
            16 * GetTileSize(),
            16 * GetTileSize(),
            semiTransparentBlack);
        DrawTextEx(m_assets.GetFont("text"), "GAME PAUSED",
                   Vector2{GetScreenWidth() / 2.0f - 500, 100},
                   200, 10, WHITE);
        DrawTextEx(m_assets.GetFont("text"), "Press P to resume the game",
                   Vector2{GetScreenWidth() / 2.0f - 242, 400},
                   50, 2, WHITE);
        return;
    }


    // 4. UI Elements (layerDepth: 0.25 - 0.5)
    // Draw UI background elements
    DrawTexturePro(
        GetTexture("cursors"),
        Rectangle{166.0f, 134.0f, 22.0f, 22.0f},
        Rectangle{m_topLeftScreenCoordinate.x - (GetTileSize() + 27),
                  m_topLeftScreenCoordinate.y,
                  66.0f, 66.0f},
        Vector2{0, 0},
        0.0f,
        WHITE);

    // Draw held item
    if (m_heldItem != nullptr)
    {
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{144.0f + m_heldItem->which * 16.0f, 160.0f, 16.0f, 16.0f},
            Rectangle{m_topLeftScreenCoordinate.x - (GetTileSize() + 18),
                      m_topLeftScreenCoordinate.y + 9.0f,
                      48.0f, 48.0f},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }

    // Draw lives icon and count
    DrawTexturePro(
        GetTexture("cursors"),
        Rectangle{272.0f, 128.0f, 16.0f, 16.0f},
        Rectangle{m_topLeftScreenCoordinate.x - GetTileSize() * 2,
                  m_topLeftScreenCoordinate.y + GetTileSize() + 18,
                  48.0f, 48.0f},
        Vector2{0, 0},
        0.0f,
        WHITE);

    DrawTextEx(m_assets.GetFont("text"),
               ("x" + std::to_string(std::max(0, m_lives))).c_str(),
               Vector2{m_topLeftScreenCoordinate.x - GetTileSize() + 8,
                       m_topLeftScreenCoordinate.y + GetTileSize() + GetTileSize() / 4 + 18},
               32, // Font size
               1,
               WHITE);

    // Draw coins icon and count
    DrawTexturePro(
        GetTexture("cursors"),
        Rectangle{144.0f, 160.0f, 16.0f, 16.0f},
        Rectangle{m_topLeftScreenCoordinate.x - GetTileSize() * 2,
                  m_topLeftScreenCoordinate.y + GetTileSize() * 2 + 18,
                  48.0f, 48.0f},
        Vector2{0, 0},
        0.0f,
        WHITE);

    DrawTextEx(m_assets.GetFont("text"),
               ("x" + std::to_string(m_coins)).c_str(),
               Vector2{m_topLeftScreenCoordinate.x - GetTileSize() + 8,
                       m_topLeftScreenCoordinate.y + GetTileSize() * 2 + GetTileSize() / 4 + 18},
               32, // Font size
               1,
               WHITE);

    // Draw wave progress indicators
    for (int i = 0; i < m_whichWave + m_whichRound * 12; i++)
    {
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{384.0f, 112.0f, 5.0f, 5.0f},
            Rectangle{m_topLeftScreenCoordinate.x + GetTileSize() * 16 + 3,
                      m_topLeftScreenCoordinate.y + static_cast<float>(i * 3 * 6),
                      15.0f, 15.0f},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }

    // Draw timer bar
    DrawTexturePro(
        GetTexture("cursors"),
        Rectangle{467.0f, 100.0f, 9.0f, 11.0f},
        Rectangle{m_topLeftScreenCoordinate.x,
                  m_topLeftScreenCoordinate.y - GetTileSize() / 2 - 12,
                  27.0f, 33.0f},
        Vector2{0, 0},
        0.0f,
        WHITE);

    if (!m_shootoutLevel)
    {
        Color timerColor = (m_waveTimer < 8000) ? Color{188, 51, 74, 255} : Color{147, 177, 38, 255};

        // Constrain the timer bar width to prevent overflowing
        int timerWidth = static_cast<int>((16 * GetTileSize() - 60) *
                                          (static_cast<float>(m_waveTimer) / WAVE_DURATION));

        // Make sure it doesn't exceed the screen width
        timerWidth = std::min(timerWidth, 16 * GetTileSize() - 60);

        DrawRectangle(
            static_cast<int>(m_topLeftScreenCoordinate.x + 30),
            static_cast<int>(m_topLeftScreenCoordinate.y - GetTileSize() / 2 + 3),
            timerWidth,
            GetTileSize() / 4,
            timerColor);
    }

    // Draw powerup indicators
    if (m_bulletDamage > 1)
    {
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{288.0f + (m_ammoLevel - 1) * 16.0f, 128.0f, 16.0f, 16.0f},
            Rectangle{m_topLeftScreenCoordinate.x - GetTileSize() - 3,
                      m_topLeftScreenCoordinate.y + 16 * GetTileSize() - GetTileSize(),
                      48.0f, 48.0f},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }

    if (m_fireSpeedLevel > 0)
    {
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{192.0f + (m_fireSpeedLevel - 1) * 16.0f, 128.0f, 16.0f, 16.0f},
            Rectangle{m_topLeftScreenCoordinate.x - GetTileSize() - 3,
                      m_topLeftScreenCoordinate.y + 16 * GetTileSize() - GetTileSize() * 2,
                      48.0f, 48.0f},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }

    if (m_runSpeedLevel > 0)
    {
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{240.0f + (m_runSpeedLevel - 1) * 16.0f, 128.0f, 16.0f, 16.0f},
            Rectangle{m_topLeftScreenCoordinate.x - GetTileSize() - 3,
                      m_topLeftScreenCoordinate.y + 16 * GetTileSize() - GetTileSize() * 3,
                      48.0f, 48.0f},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }

    if (m_spreadPistol)
    {
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{336.0f, 128.0f, 16.0f, 16.0f},
            Rectangle{m_topLeftScreenCoordinate.x - GetTileSize() - 3,
                      m_topLeftScreenCoordinate.y + 16 * GetTileSize() - GetTileSize() * 4,
                      48.0f, 48.0f},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }

    // Draw control instructions at beginning of game

    if (m_betweenWaveTimer > 0 && m_whichWave == 0 && !m_scrollingMap)
    {
        Vector2 pos = {GetScreenWidth() / 2.6, GetScreenHeight() - 138 - 3};
        Vector2 pos2 = {GetScreenWidth() / 1.89, GetScreenHeight() - 138 - 3};
        Vector2 pos3 = { GetScreenWidth() / 1.89, GetScreenHeight() - 81 - 3 };

        // Drawing controls instruction box

        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{224, 0, 80, 48},
            Rectangle{pos.x, pos.y, 240, 144},
            Vector2{0, 0},
            0.0f,
            WHITE);

        // Drawing pause instruction box

        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{119, 0, 41, 17},
            Rectangle{pos2.x, pos2.y, 150, 54},
            Vector2{0, 0},
            0.0f,
            WHITE);

        // Drawing debug instruction box

        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{ 74, 0, 45, 19 },
            Rectangle{ pos3.x, pos3.y, 151, 55 },
            Vector2{ 0, 0 },
            0.0f,
            WHITE);
    }

    // Draw screen flash effect if active
    if (m_screenFlash > 0)
    {
        DrawRectangle(
            static_cast<int>(m_topLeftScreenCoordinate.x),
            static_cast<int>(m_topLeftScreenCoordinate.y),
            16 * GetTileSize(),
            16 * GetTileSize(),
            ColorAlpha(RAYWHITE, static_cast<float>(m_screenFlash) / 1000.0f));
    }

    if (m_waitingForPlayerToMoveDownAMap && (m_merchantShopOpen || m_merchantLeaving || !m_shopping) && m_shoppingTimer < 250)
    {
        // Draw the arrow at the bottom of the screen
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{227.0f, 102.0f, 8.0f, 8.0f},
            Rectangle{m_topLeftScreenCoordinate.x + 8.5f * GetTileSize() - 12,
                      m_topLeftScreenCoordinate.y + 15.0f * GetTileSize(),
                      24.0f, 24.0f},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }

    if (m_isPaused)
    {
        // Draw semi-transparent black background
        Color semiTransparentBlack = {0, 0, 0, 128}; // Last value (128) is alpha: 0-255
        DrawRectangle(
            static_cast<int>(m_topLeftScreenCoordinate.x),
            static_cast<int>(m_topLeftScreenCoordinate.y),
            16 * GetTileSize(),
            16 * GetTileSize(),
            semiTransparentBlack);
        return;
    }

    DrawDebugInfo();
}

Rectangle PrairieKing::GetRectForShopItem(int itemID)
{
    switch (itemID)
    {
    case ITEM_FIRESPEED1:
    case ITEM_FIRESPEED2:
    case ITEM_FIRESPEED3:
        return {320.0f + (itemID % 3) * 16.0f, 1776.0f, 16.0f, 16.0f};

    case ITEM_RUNSPEED1:
    case ITEM_RUNSPEED2:
        return {368.0f + (itemID - ITEM_RUNSPEED1) * 16.0f, 1776.0f, 16.0f, 16.0f};

    case ITEM_AMMO1:
    case ITEM_AMMO2:
    case ITEM_AMMO3:
        return {416.0f + (itemID - ITEM_AMMO1) * 16.0f, 1776.0f, 16.0f, 16.0f};

    case ITEM_LIFE:
        return {400.0f, 1776.0f, 16.0f, 16.0f};

    case ITEM_SPREADPISTOL:
        return {464.0f, 1776.0f, 16.0f, 16.0f};

    case ITEM_STAR:
        return {352.0f, 1808.0f, 16.0f, 16.0f};

    case ITEM_LOG:
    case POWERUP_LOG:
        return {368.0f, 1808.0f, 16.0f, 16.0f};

    case ITEM_SKULL:
    case POWERUP_SKULL:
        return {416.0f, 1808.0f, 16.0f, 16.0f};

    case POWERUP_HEART:
        return {432.0f, 1808.0f, 16.0f, 16.0f};

    default:
        return {288.0f, 1808.0f, 16.0f, 16.0f};
    }
}

Texture2D PrairieKing::GetTexture(const std::string &name)
{
    return m_assets.GetTexture(name);
}

Sound PrairieKing::GetSound(const std::string &name)
{
    return m_assets.GetSound(name);
}

Sound PrairieKing::GetSoundStatic(const std::string &name)
{
    // Simple implementation for static context
    return LoadSound(name.c_str());
}

PrairieKing::JOTPKProgress PrairieKing::GetProgress() const
{
    PrairieKing::JOTPKProgress progress;
    progress.bulletDamage = m_bulletDamage;
    progress.fireSpeedLevel = m_fireSpeedLevel;
    progress.ammoLevel = m_ammoLevel;
    progress.spreadPistol = m_spreadPistol;
    progress.runSpeedLevel = m_runSpeedLevel;
    progress.lives = m_lives;
    progress.coins = m_coins;
    progress.score = m_score;
    progress.died = m_died;
    progress.whichRound = m_whichRound;
    progress.whichWave = m_whichWave;
    progress.heldItem = m_heldItem ? m_heldItem->which : -100;
    progress.world = m_world;
    progress.waveTimer = m_waveTimer;
    return progress;
}

void PrairieKing::AddMonster(CowboyMonster *monster)
{
    if (!IsCollidingWithMonster(monster->position, nullptr) &&
        !IsCollidingWithMapForMonsters(monster->position))
    {
        if (m_whichRound > 0)
        {
            monster->health += m_whichRound * 2;
        }

        m_monsters.push_back(monster);
        PlaySound(GetSound("cowboy_monsterhit"));
    }
    else
    {
        delete monster;
    }
}
void PrairieKing::AddTemporarySprite(const TemporaryAnimatedSprite &sprite)
{
    m_temporarySprites.push_back(sprite);
}

PrairieKing *PrairieKing::GetGameInstance()
{
    return s_instance;
}

void PrairieKing::StartNewWave()
{
    // Reset important state variables
    m_waveTimer = GameConstants::WAVE_DURATION;
    m_waveCompleted = false;
    m_died = false;
    m_betweenWaveTimer = BETWEEN_WAVE_DURATION;

    // Clear any remaining monsters
    for (auto monster : m_monsters)
    {
        delete monster;
    }
    m_monsters.clear();

    // Update monster chances
    UpdateMonsterChancesForWave();

    std::cout << "New wave started: " << m_whichWave << std::endl;

    // Determinar si vamos a tienda o siguiente nivel
    if (m_whichWave > 0)
    {
        if (m_whichWave % 2 == 0)
        {
            StartShoppingLevel();
        }
        else
        {
            m_waitingForPlayerToMoveDownAMap = true;
            m_map[8][15] = MAP_DESERT;
            m_map[7][15] = MAP_DESERT;
            m_map[9][15] = MAP_DESERT;
        }
    }

    // Reiniciar timers
    m_waveTimer = 0; // Mantener en 0 hasta que termine la transición
    m_betweenWaveTimer = BETWEEN_WAVE_DURATION;

    // Actualizar probabilidades de monstruos según la ola
    switch (m_whichWave)
    {
    case 1:
    case 2:
    case 3:
        // Olas iniciales: Orcs, Ogres y Spikeys
        m_monsterChances[0] = {0.01f + (m_whichWave - 1) * 0.005f, 0.1f + (m_whichWave - 1) * 0.05f}; // Orcs
        if (m_whichWave > 1)
        {
            m_monsterChances[2] = {0.1,0.1}; // Ogres
        }
        m_monsterChances[6] = {0.005f + (m_whichWave - 1) * 0.003f, 0.05f + (m_whichWave - 1) * 0.03f}; // Spikeys
        if (m_whichRound > 0)
        {
            m_monsterChances[4] = {0.002f, 0.1f}; // Devils en New Game Plus
        }
        break;

    case 4:
    case 5:
    case 6:
    case 7:
        // Olas medias: Ogres, Mushrooms y Ghosts
        if (m_monsterChances[5].x == 0 && m_monsterChances[5].y == 0)
        {
            m_monsterChances[5] = {0.01f, 0.15f}; // Mushrooms
            if (m_whichRound > 0)
            {
                m_monsterChances[5] = {0.01f + static_cast<float>(m_whichRound) * 0.004f,
                                       0.15f + static_cast<float>(m_whichRound) * 0.04f};
            }
        }
        m_monsterChances[0] = {0, 0};                                                                   // Desactivar Orcs
        m_monsterChances[6] = {0, 0};                                                                   // Desactivar Spikeys
        m_monsterChances[2] = {0.01f + (m_whichWave - 4) * 0.005f, 0.1f + (m_whichWave - 4) * 0.05f};   // Más Ogres
        m_monsterChances[5] = {0.01f + (m_whichWave - 4) * 0.003f, 0.15f + (m_whichWave - 4) * 0.03f};  // Más Mushrooms
        m_monsterChances[1] = {0.005f + (m_whichWave - 4) * 0.003f, 0.05f + (m_whichWave - 4) * 0.03f}; // Más Ghosts
        if (m_whichRound > 0)
        {
            m_monsterChances[4] = {0.001f, 0.1f}; // Devils en New Game Plus
        }
        break;

    case 8:
    case 9:
    case 10:
    case 11:
        // Olas avanzadas: Mummies y Devils
        m_monsterChances[5] = {0, 0}; // Desactivar Mushrooms
        m_monsterChances[1] = {0, 0}; // Desactivar Ghosts
        m_monsterChances[2] = {0, 0}; // Desactivar Ogres
        if (m_monsterChances[3].x == 0 && m_monsterChances[3].y == 0)
        {
            m_monsterChances[3] = {0.012f, 0.4f}; // Mummies
            if (m_whichRound > 0)
            {
                m_monsterChances[3] = {0.012f + static_cast<float>(m_whichRound) * 0.005f,
                                       0.4f + static_cast<float>(m_whichRound) * 0.075f};
            }
        }
        if (m_monsterChances[4].x == 0 && m_monsterChances[4].y == 0)
        {
            m_monsterChances[4] = {0.003f, 0.1f}; // Devils
        }
        m_monsterChances[3] = {0.012f + (m_whichWave - 8) * 0.005f, 0.4f + (m_whichWave - 8) * 0.05f}; // Más Mummies
        m_monsterChances[4] = {0.003f + (m_whichWave - 8) * 0.003f, 0.1f + (m_whichWave - 8) * 0.03f}; // Más Devils
        if (m_whichWave == 11)
        {
            m_monsterChances[4] = {m_monsterChances[4].x + 0.01f, m_monsterChances[4].y + 0.04f}; // Aún más Devils
            m_monsterChances[3] = {m_monsterChances[3].x - 0.01f, m_monsterChances[3].y + 0.04f}; // Menos Mummies
        }
        break;

    case 12:
        // Ola final: Jefe
        // Limpiar todas las probabilidades de monstruos normales
        for (auto &chance : m_monsterChances)
        {
            chance = {0, 0};
        }
        // El jefe se maneja en otra parte del código
        break;
    }

    // Aumentar dificultad en New Game Plus
    if (m_whichRound > 0)
    {
        for (auto &chance : m_monsterChances)
        {
            chance.x *= 1.1f;
            chance.y *= 1.1f;
        }
    }
}

void PrairieKing::UpdatePlayer(float deltaTime)
{
    // No actualizar si el jugador está muerto
    if (m_deathTimer > 0.0f)
    {
        m_deathTimer -= deltaTime * 1000.0f;
        return;
    }

    // Recolectar power-ups si el jugador está sobre ellos
    for (int i = m_powerups.size() - 1; i >= 0; i--)
    {
        if (CheckCollisionRecs(
                m_playerBoundingBox,
                Rectangle{m_powerups[i].position.x, m_powerups[i].position.y,
                          static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}) &&
            !CheckCollisionRecs(m_playerBoundingBox, m_noPickUpBox))
        {
            if (m_heldItem != nullptr)
            {
                UsePowerup(m_powerups[i].which);
                m_powerups.erase(m_powerups.begin() + i);
            }
            else if (GetPowerUp(m_powerups[i]))
            {
                m_powerups.erase(m_powerups.begin() + i);
            }
        }
    }

    // Limpiar la caja de no recoger si el jugador no está colisionando con ella
    if (!CheckCollisionRecs(m_playerBoundingBox, m_noPickUpBox))
    {
        m_noPickUpBox = {0.0f, 0.0f, static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())};
    }

    // Manejar movimiento
    if (!m_playerMovementDirections.empty() && !m_scrollingMap && m_deathTimer <= 0.0f)
    {
        // Calcular direcciones efectivas para el movimiento
        int effectiveDirections = m_playerMovementDirections.size();
        if (effectiveDirections >= 2 &&
            m_playerMovementDirections.back() == (m_playerMovementDirections[m_playerMovementDirections.size() - 2] + 2) % 4)
        {
            effectiveDirections = 1;
        }

        // Calcular velocidad base
        float speed = GetMovementSpeed(PLAYER_SPEED, effectiveDirections);

        if (m_activePowerups.find(6) != m_activePowerups.end())
        {
            speed *= 1.5f;
        }

        if (m_zombieModeTimer > 0)
        {
            speed *= 1.5f;
        }

        // Aplicar bonus de nivel de velocidad
        for (int i = 0; i < m_runSpeedLevel; i++)
        {
            speed *= 1.25f;
        }

        // Mover jugador en cada dirección (manejando pulsaciones simultáneas)
        for (int i = std::max(0, static_cast<int>(m_playerMovementDirections.size()) - 2);
             i < static_cast<int>(m_playerMovementDirections.size()); i++)
        {
            // Saltar si causaría cancelación con dirección opuesta
            if (i != 0 || m_playerMovementDirections.size() < 2 ||
                m_playerMovementDirections.back() != (m_playerMovementDirections[m_playerMovementDirections.size() - 2] + 2) % 4)
            {
                Vector2 newPosition = m_playerPosition;

                // Aplicar movimiento según dirección
                switch (m_playerMovementDirections[i])
                {
                case 0: // Arriba
                    newPosition.y -= speed;
                    break;
                case 1: // Derecha
                    newPosition.x += speed;
                    break;
                case 2: // Abajo
                    newPosition.y += speed;
                    break;
                case 3: // Izquierda
                    newPosition.x -= speed;
                    break;
                }

                // Actualizar caja de colisión para verificación
                Rectangle newBounds = {
                    newPosition.x + static_cast<float>(GetTileSize()) / 4.0f,
                    newPosition.y + static_cast<float>(GetTileSize()) / 4.0f,
                    static_cast<float>(GetTileSize()) / 2.0f,
                    static_cast<float>(GetTileSize()) / 2.0f};

                // Verificar si la nueva posición es válida
                if (!IsCollidingWithMap(newBounds) &&
                    (!CheckCollisionRecs(newBounds, m_merchantBox) || CheckCollisionRecs(m_playerBoundingBox, m_merchantBox)))
                {
                    m_playerPosition = newPosition;
                    m_playerBoundingBox = newBounds;
                }
            }
        }

        // Actualizar caja de colisión con nueva posición
        m_playerBoundingBox = {
            m_playerPosition.x + static_cast<float>(GetTileSize()) / 4.0f,
            m_playerPosition.y + static_cast<float>(GetTileSize()) / 4.0f,
            static_cast<float>(GetTileSize()) / 2.0f,
            static_cast<float>(GetTileSize()) / 2.0f};

        // Actualizar timers de animación
        if (!m_playerMovementDirections.empty())
        {
            m_playerMotionAnimationTimer += deltaTime * 1000.0f;
            if (m_playerMotionAnimationTimer >= 400.0f)
            {
                m_playerMotionAnimationTimer = 0.0f;
            }

            // Reproducir sonidos de pasos
            m_playerFootstepSoundTimer -= deltaTime * 1000.0f;
            if (m_playerFootstepSoundTimer <= 0.0f)
            {
                PlaySound(GetSound("cowboy_footstep"));
                m_playerFootstepSoundTimer = 200.0f;
            }
        }
        else
        {
            // Reiniciar timer de animación cuando no se mueve
            m_playerMotionAnimationTimer = 0.0f;
        }

        // Verificar transiciones de mapa
        if (m_waitingForPlayerToMoveDownAMap &&
            CheckCollisionRecs(m_playerBoundingBox, Rectangle{8.5f * GetTileSize() - 12, 15.0f * GetTileSize(), 24.0f, 24.0f}))
        {
            std::cout << "Player collided with arrow. Transitioning to next map." << std::endl;
            SaveGame();
            m_shopping = false;
            m_merchantArriving = false;
            m_merchantLeaving = false;
            m_merchantShopOpen = false;
            m_scrollingMap = true;

            // Generar siguiente mapa al iniciar scroll
            m_newMapPosition = 16 * GetTileSize();
            m_temporarySprites.clear();
            m_powerups.clear();
        }
    }

    // Verificar colisiones con monstruos
    for (int i = m_monsters.size() - 1; i >= 0; i--)
    {
        if (CheckCollisionRecs(m_monsters[i]->position, m_playerBoundingBox) && m_playerInvincibleTimer <= 0)
        {
            if (m_zombieModeTimer <= 0)
            {
                PlayerDie();
                break;
            }
            else if (m_monsters[i]->type != -2) // No es jefe
            {
                // Matar monstruo en modo zombie
                AddGuts({m_monsters[i]->position.x, m_monsters[i]->position.y}, m_monsters[i]->type);
                delete m_monsters[i];
                m_monsters.erase(m_monsters.begin() + i);
                PlaySound(GetSound("Cowboy_monsterDie"));
            }
        }
    }

    // Manejar disparos
    if (m_shotTimer > 0)
    {
        m_shotTimer -= static_cast<int>(deltaTime * 1000.0f);
    }

    if (m_deathTimer <= 0.0f && !m_playerShootingDirections.empty() && m_shotTimer <= 0)
    {
        // Caso especial para power-up de disparo extendido
        if (m_activePowerups.find(POWERUP_SPREAD) != m_activePowerups.end())
        {
            // Disparar en las 8 direcciones
            std::vector<int> dirs;

            // Direcciones cardinales
            dirs = {0}; // Arriba
            SpawnBullets(dirs, m_playerPosition);

            dirs = {1}; // Derecha
            SpawnBullets(dirs, m_playerPosition);

            dirs = {2}; // Abajo
            SpawnBullets(dirs, m_playerPosition);

            dirs = {3}; // Izquierda
            SpawnBullets(dirs, m_playerPosition);

            // Diagonales
            dirs = {0, 1}; // Arriba-Derecha
            SpawnBullets(dirs, m_playerPosition);

            dirs = {1, 2}; // Abajo-Derecha
            SpawnBullets(dirs, m_playerPosition);

            dirs = {2, 3}; // Abajo-Izquierda
            SpawnBullets(dirs, m_playerPosition);

            dirs = {3, 0}; // Arriba-Izquierda
            SpawnBullets(dirs, m_playerPosition);
        }
        else if (m_playerShootingDirections.size() == 1 ||
                 m_playerShootingDirections.back() == (m_playerShootingDirections[m_playerShootingDirections.size() - 2] + 2) % 4)
        {
            // Una dirección o direcciones que se cancelan - disparar en la última dirección
            int dirIndex = (m_playerShootingDirections.size() == 2 &&
                            m_playerShootingDirections.back() == (m_playerShootingDirections[m_playerShootingDirections.size() - 2] + 2) % 4)
                               ? 1
                               : 0;

            std::vector<int> dir = {m_playerShootingDirections[dirIndex]};
            SpawnBullets(dir, m_playerPosition);
        }
        else
        {
            // Múltiples direcciones - disparar en diagonal
            SpawnBullets(m_playerShootingDirections, m_playerPosition);
        }

        // Reproducir sonido de disparo
        PlaySound(GetSound("cowboy_gunshot"));

        // Establecer cooldown de disparo
        m_shotTimer = m_shootingDelay;

        // Aplicar power-up de disparo rápido
        if (m_activePowerups.find(POWERUP_RAPIDFIRE) != m_activePowerups.end())
        {
            m_shotTimer /= 4;
        }

        // Aplicar bonus de nivel de velocidad de disparo
        for (int i = 0; i < m_fireSpeedLevel; i++)
        {
            m_shotTimer = m_shotTimer * 3 / 4;
        }

        // Aplicar penalización de escopeta
        if (m_activePowerups.find(POWERUP_SHOTGUN) != m_activePowerups.end())
        {
            m_shotTimer = m_shotTimer * 3 / 2;
        }

        // Asegurar timer mínimo de disparo
        m_shotTimer = std::max(m_shotTimer, 20);
    }
}

std::vector<Vector2> PrairieKing::GetBorderPoints()
{
    std::vector<Vector2> points;
    const int tileSize = GetTileSize();

    // Add top and bottom border points
    for (int x = 1; x < MAP_WIDTH - 1; x++)
    {
        // Top border
        points.push_back({static_cast<float>(x * tileSize), 0.0f});
        // Bottom border
        points.push_back({static_cast<float>(x * tileSize), static_cast<float>((MAP_HEIGHT - 1) * tileSize)});
    }

    // Add left and right border points
    for (int y = 1; y < MAP_HEIGHT - 1; y++)
    {
        // Left border
        points.push_back({0.0f, static_cast<float>(y * tileSize)});
        // Right border
        points.push_back({static_cast<float>((MAP_WIDTH - 1) * tileSize), static_cast<float>(y * tileSize)});
    }

    return points;
}

// Implementación de los constructores de CowboyMonster
PrairieKing::CowboyMonster::CowboyMonster(AssetManager &assets, int which, Vector2 position)
    : type(which), position({position.x, position.y, 16.0f * 3, 16.0f * 3})
{
    // Inicializar salud y velocidad según el tipo de monstruo
    switch (type)
    {
    case GameConstants::ORC:
        health = 1;   // Los orcos deben morir con un disparo
        speed = 2.0f; // Velocidad base para los orcos
        break;

    case GameConstants::SPIKEY:
        health = 2;   // Los Spikey tienen salud moderada
        speed = 1.8f; // Velocidad ligeramente superior
        break;

    case GameConstants::OGRE:
        health = 3;   // Los ogros son más resistentes
        speed = 1.0f; // Velocidad más lenta para los ogros
        break;

    case GameConstants::MUSHROOM:
        health = 2;
        speed = 2.1f;

    case GameConstants::EVIL_BUTTERFLY:
        health = 2;   // Los fantasmas tienen salud moderada
        speed = 2.0f; // Los fantasmas son rápidos
        break;

    case GameConstants::MUMMY:
        health = 4;   // Las momias son muy resistentes
        speed = 0.8f; // Las momias son lentas
        break;

    case GameConstants::IMP:
        health = 5;   // Los demonios son muy resistentes
        speed = 2.5f; // Los demonios son muy rápidos
        break;


    default:
        health = 100; // Valor por defecto para otros tipos
        speed = 1.0f; // Velocidad por defecto
        break;
    }

    // Inicializar otros atributos
    movementAnimationTimer = 0.0f;
    movementDirection = 0;
    movedLastTurn = false;
    oppositeMotionGuy = false;
    invisible = false;
    special = false;
    uninterested = false;
    flyer = false;
    tint = WHITE;
    flashColor = WHITE;
    flashColorTimer = 0.0f;
    ticksSinceLastMovement = 0;
    acceleration = {0, 0};
    targetPosition = position;
}

// Implementación de los métodos virtuales de CowboyMonster
void PrairieKing::CowboyMonster::Draw(const Texture2D &texture, Vector2 topLeftScreenCoordinate)
{
    // Don't draw if the monster is invisible
    if (invisible && type != GameConstants::SPIKEY)
        return;

    Rectangle destRect = {
        topLeftScreenCoordinate.x + position.x,
        topLeftScreenCoordinate.y + position.y,
        position.width,
        position.height};

    // Determine the texture rectangle based on monster type
    Rectangle sourceRect;

    if (type == GameConstants::SPIKEY && special)
    {
        // Special Spikey has different sprites
        if (flashColorTimer > 0.0f)
        {
            sourceRect = {480.0f, 112.0f, 16.0f, 16.0f};
        }
        else
        {
            sourceRect = {576.0f, 128.0f, 16.0f, 16.0f};
        }
    }
    else if (!invisible)
    {
        // Normal monsters
        if (flashColorTimer > 0.0f)
        {
            // Hit animation
            sourceRect = {static_cast<float>(224 + type * 16), 48.0f, 16.0f, 16.0f};
        }
        else
        {
            // Walking animation
            sourceRect = {static_cast<float>(224 + (type * 2 + ((movementAnimationTimer < 250.0f) ? 1 : 0)) * 16), 64.0f, 16.0f, 16.0f};
        }
        
        // Draw the monster
        DrawTexturePro(texture, sourceRect, destRect, Vector2{0, 0}, 0.0f, WHITE);
        
        // Draw confusion indicator if monster is confused
        if (PrairieKing::GetGameInstance()->m_monsterConfusionTimer > 0)
        {
            // Get font and text measurements from the asset manager
            Font smallFont = PrairieKing::GetGameInstance()->m_assets.GetFont("small");
            const char* text = "?";
            Vector2 textSize = MeasureTextEx(smallFont, text, 16.0f, 1.0f);
            
            // Position for the question mark
            Vector2 textPos = {
                topLeftScreenCoordinate.x + position.x + position.width/2 - textSize.x/2,
                topLeftScreenCoordinate.y + position.y - PrairieKing::GetGameInstance()->GetTileSize()/2
            };
            
            // Draw with "drop shadow" effect
            Color confusionColor = {88, 29, 43, 255}; // Dark red color
            
            // Draw question mark with slight offsets for outline effect
            DrawTextEx(smallFont, text, Vector2{textPos.x, textPos.y}, 16.0f, 1.0f, confusionColor);
            DrawTextEx(smallFont, text, Vector2{textPos.x + 1.0f, textPos.y}, 16.0f, 1.0f, confusionColor);
            DrawTextEx(smallFont, text, Vector2{textPos.x - 1.0f, textPos.y}, 16.0f, 1.0f, confusionColor);
        }
    }
}

bool PrairieKing::CowboyMonster::TakeDamage(int damage)
{
    if (invisible)
        return false;

    health -= damage;
    flashColorTimer = 0.3f;
    flashColor = RED;


    PlaySound(PrairieKing::GetGameInstance()->GetSound("cowboy_monsterhit"));

    if (health <= 0)
    {
        // Drop loot
        int lootDrop = GetLootDrop();
        if (lootDrop != -1)
        {
            // Add powerup at monster position
            PrairieKing::GetGameInstance()->m_powerups.push_back(
                CowboyPowerup(lootDrop, Vector2{position.x, position.y}, 10000));
        }

        // Add guts animation
        PrairieKing::AddGuts(Vector2{position.x, position.y}, type);

        return true;
    }

    return false;
}

int PrairieKing::CowboyMonster::GetLootDrop()
{
    // Special case for special spikey enemy
    if (type == GameConstants::SPIKEY && special)
    {
        return -1; // No drop
    }

    // Base 5% chance for initial loot roll
    if (GetRandomFloat(0.0f, 1.0f) < 0.05f)
    {
        // For non-basic enemies, 10% chance for coin
        if (type != GameConstants::ORC && GetRandomFloat(0.0f, 1.0f) < 0.1f)
        {
            return COIN1;
        }

        // 1% chance for coin
        if (GetRandomFloat(0.0f, 1.0f) < 0.06f)
        {
            return COIN1;
        }

        return 0; // Basic powerup
    }

    // Another 5% chance for different powerups
    if (GetRandomFloat(0.0f, 1.0f) < 0.05f)
    {
        // 15% chance for special powerups
        if (GetRandomFloat(0.0f, 1.0f) < 0.15f)
        {
            return GetRandomInt(6, 7);
        }

        // 7% chance for sheriff star
        if (GetRandomFloat(0.0f, 1.0f) < 0.07f)
        {
            return 10;
        }

        // Random powerup between 2-9
        int loot = GetRandomInt(1, 9);

        // 40% chance to reroll if got powerup 5
        if (loot == 5 && GetRandomFloat(0.0f, 1.0f) < 0.4f)
        {
            loot = GetRandomInt(1, 9);
        }

        return loot;
    }

    return -1; // No loot drop
}

bool PrairieKing::CowboyMonster::Move(Vector2 playerPosition, float deltaTime)
{
    // Update the movement animation timer
    movementAnimationTimer -= deltaTime * 1000.0f;
    if (movementAnimationTimer <= 0.0f)
    {
        movementAnimationTimer = std::max(100.0f, 500.0f - speed * 50.0f);
    }

    // If the monster is flashing, don't move
    if (flashColorTimer > 0.0f)
    {
        flashColorTimer -= deltaTime;
        //return false;
    }

    // Important fix: Don't return here, just update the timer and continue
    // This allows confused monsters to still move, but randomly
    if (PrairieKing::GetGameInstance()->m_monsterConfusionTimer > 0)
    {
        // Update the timer but don't return
        PrairieKing::GetGameInstance()->m_monsterConfusionTimer -= deltaTime * 1000.0f;
        
        // For confused monsters, occasionally change direction
        if (GetRandomFloat(0.0f, 1.0f) < 0.05f)
        {
            oppositeMotionGuy = !oppositeMotionGuy;
        }
    }

    // Increment the counter of ticks since last movement
    ticksSinceLastMovement++;

    // Specific behavior based on monster type
    switch (type)
    {
    case GameConstants::ORC:
    case GameConstants::OGRE:
    case GameConstants::MUMMY:
    case GameConstants::MUSHROOM:
    case GameConstants::SPIKEY:
    {
        // Special behavior for Spikey
        if (type == GameConstants::SPIKEY)
        {
            if (special || invisible)
            {
                break;
            }
            if (ticksSinceLastMovement > 20)
            {
                int tries = 0;
                do
                {
                    targetPosition = {
                        static_cast<float>(GetRandomInt(2, 14) * PrairieKing::GetGameInstance()->GetTileSize()),
                        static_cast<float>(GetRandomInt(2, 14) * PrairieKing::GetGameInstance()->GetTileSize())};
                    tries++;
                } while (PrairieKing::GetGameInstance()->IsCollidingWithMap(targetPosition) && tries < 5);
            }
        }
        else if (ticksSinceLastMovement > 20)
        {
            int tries = 0;
            do
            {
                oppositeMotionGuy = !oppositeMotionGuy;
                targetPosition = {
                    static_cast<float>(GetRandomInt(
                        static_cast<int>(position.x) - PrairieKing::GetGameInstance()->GetTileSize() * 2,
                        static_cast<int>(position.x) + PrairieKing::GetGameInstance()->GetTileSize() * 2)),
                    static_cast<float>(GetRandomInt(
                        static_cast<int>(position.y) - PrairieKing::GetGameInstance()->GetTileSize() * 2,
                        static_cast<int>(position.y) + PrairieKing::GetGameInstance()->GetTileSize() * 2))};
                tries++;
            } while (PrairieKing::GetGameInstance()->IsCollidingWithMap(targetPosition) && tries < 5);
        }

        // Determine the target
        Vector2 target = (targetPosition.x != 0.0f || targetPosition.y != 0.0f) ? targetPosition : playerPosition;

        // If the gopher is running, chase the gopher instead
        if (PrairieKing::GetGameInstance()->m_gopherRunning)
        {
            target = {PrairieKing::GetGameInstance()->m_gopherBox.x, PrairieKing::GetGameInstance()->m_gopherBox.y};
        }

        // Occasionally change movement direction
        if (GetRandomFloat(0.0f, 1.0f) < 0.001f)
        {
            oppositeMotionGuy = !oppositeMotionGuy;
        }

        // Determine movement direction
        if ((type == GameConstants::SPIKEY && !oppositeMotionGuy) ||
            std::abs(target.x - position.x) > std::abs(target.y - position.y))
        {
            if (target.x + speed < position.x && (movedLastTurn || movementDirection != 3))
            {
                movementDirection = 3; // Left
            }
            else if (target.x > position.x + speed && (movedLastTurn || movementDirection != 1))
            {
                movementDirection = 1; // Right
            }
            else if (target.y > position.y + speed && (movedLastTurn || movementDirection != 2))
            {
                movementDirection = 2; // Down
            }
            else if (target.y + speed < position.y && (movedLastTurn || movementDirection != 0))
            {
                movementDirection = 0; // Up
            }
        }
        else
        {
            if (target.y > position.y + speed && (movedLastTurn || movementDirection != 2))
            {
                movementDirection = 2; // Down
            }
            else if (target.y + speed < position.y && (movedLastTurn || movementDirection != 0))
            {
                movementDirection = 0; // Up
            }
            else if (target.x + speed < position.x && (movedLastTurn || movementDirection != 3))
            {
                movementDirection = 3; // Left
            }
            else if (target.x > position.x + speed && (movedLastTurn || movementDirection != 1))
            {
                movementDirection = 1; // Right
            }
        }

        movedLastTurn = false;
        Rectangle attemptedPosition = position;

        // Apply movement based on direction
        switch (movementDirection)
        {
        case 0: // Up
            attemptedPosition.y -= speed;
            break;
        case 1: // Right
            attemptedPosition.x += speed;
            break;
        case 2: // Down
            attemptedPosition.y += speed;
            break;
        case 3: // Left
            attemptedPosition.x -= speed;
            break;
        }

        // In zombie mode, invert the direction
        if (PrairieKing::GetGameInstance()->m_zombieModeTimer > 0)
        {
            attemptedPosition.x = position.x - (attemptedPosition.x - position.x);
            attemptedPosition.y = position.y - (attemptedPosition.y - position.y);
        }

        // Special behavior for Ogre (type 2)
        if (type == GameConstants::OGRE)
        {
            for (int i = PrairieKing::GetGameInstance()->m_monsters.size() - 1; i >= 0; i--)
            {
                auto *monster = PrairieKing::GetGameInstance()->m_monsters[i];
                if (monster->type == GameConstants::SPIKEY && monster->special &&
                    CheckCollisionRecs(attemptedPosition, monster->position))
                {
                    PrairieKing::AddGuts({monster->position.x, monster->position.y}, monster->type);
                    PlaySound(PrairieKing::GetGameInstance()->GetSound("Cowboy_monsterDie"));
                    delete monster;
                    PrairieKing::GetGameInstance()->m_monsters.erase(
                        PrairieKing::GetGameInstance()->m_monsters.begin() + i);
                }
            }
        }

        // Check for collisions
        if (PrairieKing::GetGameInstance()->IsCollidingWithMapForMonsters(attemptedPosition) ||
            PrairieKing::GetGameInstance()->IsCollidingWithMonster(attemptedPosition, this) ||
            PrairieKing::GetGameInstance()->m_deathTimer > 0.0f)
        {
            break;
        }

        // Update position if no collisions
        ticksSinceLastMovement = 0;
        position = attemptedPosition;
        movedLastTurn = true;

        // Check if we reached the target
        if (!CheckCollisionPointRec({target.x + PrairieKing::GetGameInstance()->GetTileSize() / 2.0f,
                                    target.y + PrairieKing::GetGameInstance()->GetTileSize() / 2.0f},
                                   position))
        {
            break;
        }

        // Reset target
        targetPosition = {0.0f, 0.0f};

        // Special behavior for Orc and Mummy
        if ((type == GameConstants::ORC || type == GameConstants::MUMMY) && uninterested)
        {
            targetPosition = {
                static_cast<float>(GetRandomInt(2, 14) * PrairieKing::GetGameInstance()->GetTileSize()),
                static_cast<float>(GetRandomInt(2, 14) * PrairieKing::GetGameInstance()->GetTileSize())};
            if (GetRandomFloat(0.0f, 1.0f) < 0.5f)
            {
                uninterested = false;
                targetPosition = {0.0f, 0.0f};
            }
        }

        // Critical fix: Add Spikey invisibility behavior
        //if (type == GameConstants::SPIKEY && !invisible)
        //{
        //    // Create temporary sprite animation for Spikey disappearing
        //    PrairieKing::TemporaryAnimatedSprite sprite(
        //        Rectangle{352, 144, 16, 16}, 60.0f, 3, 0,
        //        Vector2{position.x + PrairieKing::GetGameInstance()->m_topLeftScreenCoordinate.x, 
        //               position.y + PrairieKing::GetGameInstance()->m_topLeftScreenCoordinate.y},
        //        0.0f, 3.0f, false, 
        //        position.y / 10000.0f, WHITE);
        //    
        //    // Add end function to restore Spikey behavior
        //    sprite.endFunction = [](int extraData) {
        //        // This would handle what happens when Spikey animation ends
        //        // In the original, it would make Spikey "special"
        //    };
        //    
        //    PrairieKing::GetGameInstance()->AddTemporarySprite(sprite);
        //    invisible = true;
        //}
        break;
    }
    case GameConstants::EVIL_BUTTERFLY:
    case GameConstants::IMP:
    {
        // Behavior for flying monsters
        if (ticksSinceLastMovement > 20)
        {
            int tries = 0;
            do
            {
                oppositeMotionGuy = !oppositeMotionGuy;
                targetPosition = {
                    static_cast<float>(GetRandomInt(
                        static_cast<int>(position.x) - PrairieKing::GetGameInstance()->GetTileSize() * 2,
                        static_cast<int>(position.x) + PrairieKing::GetGameInstance()->GetTileSize() * 2)),
                    static_cast<float>(GetRandomInt(
                        static_cast<int>(position.y) - PrairieKing::GetGameInstance()->GetTileSize() * 2,
                        static_cast<int>(position.y) + PrairieKing::GetGameInstance()->GetTileSize() * 2))};
                tries++;
            } while (PrairieKing::GetGameInstance()->IsCollidingWithMap(targetPosition) && tries < 5);
        }

        // Determine the target
        Vector2 target = (targetPosition.x != 0.0f || targetPosition.y != 0.0f) ? targetPosition : playerPosition;

        // Calculate velocity to target
        Vector2 targetToFly = GetVelocityTowardPoint(
            {position.x, position.y},
            {target.x + PrairieKing::GetGameInstance()->GetTileSize() / 2.0f,
             target.y + PrairieKing::GetGameInstance()->GetTileSize() / 2.0f},
            speed);

        // Adjust acceleration
        float accelerationMultiplier = (targetToFly.x != 0.0f && targetToFly.y != 0.0f) ? 1.5f : 1.0f;

        if (targetToFly.x > acceleration.x)
        {
            acceleration.x += 0.1f * accelerationMultiplier;
        }
        if (targetToFly.x < acceleration.x)
        {
            acceleration.x -= 0.1f * accelerationMultiplier;
        }
        if (targetToFly.y > acceleration.y)
        {
            acceleration.y += 0.1f * accelerationMultiplier;
        }
        if (targetToFly.y < acceleration.y)
        {
            acceleration.y -= 0.1f * accelerationMultiplier;
        }

        // Calculate new position and check collisions
        Rectangle newPosition = {
            position.x + static_cast<int>(std::ceil(acceleration.x)),
            position.y + static_cast<int>(std::ceil(acceleration.y)),
            position.width,
            position.height};

        // Update position if no collisions
        if (!PrairieKing::GetGameInstance()->IsCollidingWithMonster(newPosition, this) &&
            PrairieKing::GetGameInstance()->m_deathTimer <= 0.0f)
        {
            ticksSinceLastMovement = 0;
            position.x += static_cast<int>(std::ceil(acceleration.x));
            position.y += static_cast<int>(std::ceil(acceleration.y));

            // Check if reached target
            if (CheckCollisionPointRec({target.x + PrairieKing::GetGameInstance()->GetTileSize() / 2.0f,
                                       target.y + PrairieKing::GetGameInstance()->GetTileSize() / 2.0f},
                                      position))
            {
                targetPosition = {0.0f, 0.0f};
            }
        }
        break;
    }
    }

    return false;
}

// Función auxiliar para calcular la velocidad hacia un punto
Vector2 PrairieKing::CowboyMonster::GetVelocityTowardPoint(Vector2 start, Vector2 end, float speed)
{
    Vector2 direction = {end.x - start.x, end.y - start.y};
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0)
    {
        direction.x /= length;
        direction.y /= length;
    }

    return {direction.x * speed, direction.y * speed};
}

// Función auxiliar para generar números aleatorios
int PrairieKing::CowboyMonster::GetRandomInt(int min, int max)
{
    return min + (std::rand() % (max - min + 1));
}

float PrairieKing::CowboyMonster::GetRandomFloat(float min, float max)
{
    return min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

std::vector<Vector2> PrairieKing::GetMonsterChancesForWave(int wave)
{
    std::vector<Vector2> chances;

    // Base chances for each monster type
    float orcChance = 0.4f;
    float ogreChance = 0.3f;
    float spikeyChance = 0.3f;
    float mushroomChance = 0.6f;
    float evilButterflyChance = 0.4f;
    float mummyChance = 0.6f;
    float impChance = 0.4f;

    switch (wave)
    {
    case 0:
        orcChance = std::max(0.1f, orcChance - wave * 0.05f);
        chances.push_back({ ORC, orcChance });

        break;
    case 1:
        orcChance = std::max(0.1f, orcChance - wave * 0.05f);
        spikeyChance = std::min(0.9f, spikeyChance + wave * 0.02f);
        chances.push_back({ ORC, orcChance });
        chances.push_back({ SPIKEY, spikeyChance });
 
        break;
    case 2:
    case 3:
        orcChance = std::max(0.2f, orcChance - wave * 0.05f);
        ogreChance = std::min(0.4f, ogreChance + wave * 0.03f);
        spikeyChance = std::min(0.4f, spikeyChance + wave * 0.02f);
        chances.push_back({ ORC, orcChance });
        chances.push_back({ OGRE, ogreChance });
        chances.push_back({ SPIKEY, spikeyChance });
        break;
    case 4: // BOSS
        break;
    case 5:
    case 6:
    case 7:
        mushroomChance = std::max(0.6f, mushroomChance + wave * 0.1f);
        evilButterflyChance = std::min(0.4f, evilButterflyChance + wave * 0.1f);
        chances.push_back({ MUSHROOM, mushroomChance });
        chances.push_back({ EVIL_BUTTERFLY, evilButterflyChance });
        break;
    case 8: //BOSS
        break;
    case 9:
    case 10:
    case 11:
        mummyChance = std::max(0.6f, mummyChance + wave * 0.1f);
        impChance = std::min(0.4f, impChance + wave * 0.1f);
        chances.push_back({ MUMMY,  mummyChance });
        chances.push_back({ IMP, impChance });
        break;
    case 12: //BOSS
        break;
    default:
        break;
    }




    return chances;
}

Vector2 PrairieKing::GetRandomSpawnPosition()
{
    const int tileSize = GetTileSize();
    std::vector<Vector2> sideSpawnPositions = {
        {0.0f, 6.0f * tileSize}, {0.0f, 7.0f * tileSize}, {0.0f, 8.0f * tileSize}, {15.0f * tileSize, 6.0f * tileSize}, {15.0f * tileSize, 7.0f * tileSize}, {15.0f * tileSize, 8.0f * tileSize}, {6.0f * tileSize, 0.0f}, {7.0f * tileSize, 0.0f}, {8.0f * tileSize, 0.0f}, {6.0f * tileSize, 15.0f * tileSize}, {7.0f * tileSize, 15.0f * tileSize}, {8.0f * tileSize, 15.0f * tileSize}};

    for (int attempts = 0; attempts < 10; attempts++)
    {
        Vector2 pos = sideSpawnPositions[GetRandomInt(0, sideSpawnPositions.size() - 1)];
        if (IsMapTilePassable(m_map[static_cast<int>(pos.x / tileSize)][static_cast<int>(pos.y / tileSize)]) &&
            !IsCollidingWithMap(pos))
        {
            return pos;
        }
    }

    return sideSpawnPositions[GetRandomInt(0, sideSpawnPositions.size() - 1)];
}

int PrairieKing::ChooseMonsterType(const std::vector<Vector2> &chances)
{
    float total = 0.0f;
    for (const auto &chance : chances)
    {
        total += chance.y;
    }

    float roll = GetRandomFloat(0.0f, total);
    float current = 0.0f;

    for (const auto &chance : chances)
    {
        current += chance.y;
        if (roll <= current)
        {
            return static_cast<int>(chance.x);
        }
    }

    //return ORC; // Default to orc if something goes wrong
}
int PrairieKing::GetRandomInt(int min, int max)
{
    return min + (std::rand() % (max - min + 1));
}

void PrairieKing::HandleDebugInputs()
{
    // Toggle debug mode with F3
    if (IsKeyPressed(GameKeys::DebugToggle))
    {
        m_debugMode = !m_debugMode;
        std::cout << "Debug mode: " << (m_debugMode ? "ON" : "OFF") << std::endl;
        return;
    }

    // Only process other debug inputs if debug mode is enabled
    if (!m_debugMode)
        return;

    // Debug spawn monsters (Numpad 1-7)
    if (IsKeyPressed(GameKeys::DebugSpawn1))
        SpawnDebugMonster(ORC);
    if (IsKeyPressed(GameKeys::DebugSpawn2))
        SpawnDebugMonster(EVIL_BUTTERFLY);
    if (IsKeyPressed(GameKeys::DebugSpawn3))
        SpawnDebugMonster(OGRE);
    if (IsKeyPressed(GameKeys::DebugSpawn4))
        SpawnDebugMonster(MUMMY);
    if (IsKeyPressed(GameKeys::DebugSpawn5))
        SpawnDebugMonster(IMP);
    if (IsKeyPressed(GameKeys::DebugSpawn6))
        SpawnDebugMonster(MUSHROOM);
    if (IsKeyPressed(GameKeys::DebugSpawn7))
        SpawnDebugMonster(SPIKEY);

    // Debug spawn powerups (Numpad 8-9)
    if (IsKeyPressed(GameKeys::DebugSpawn8))
        SpawnDebugPowerup(GetRandomInt(0, 5));
    if (IsKeyPressed(GameKeys::DebugSpawn9))
        SpawnDebugPowerup(GetRandomInt(6, 10));

    // Other debug controls
    if (IsKeyDown(GameKeys::DebugAddLife))
    {
        m_lives++;
        std::cout << "Lives increased to: " << m_lives << std::endl;
    }
    if (IsKeyDown(GameKeys::DebugAddCoins))
    {
        m_coins += 10;
        std::cout << "Added 10 coins. Total: " << m_coins << std::endl;
    }
    if (IsKeyDown(GameKeys::DebugIncDamage))
    {
        m_bulletDamage++;
        std::cout << "Bullet damage increased to: " << m_bulletDamage << std::endl;
    }
    if (IsKeyDown(GameKeys::DebugClearMonsters))
    {
        int count = m_monsters.size();
        for (auto monster : m_monsters)
        {
            delete monster;
        }
        m_monsters.clear();
        std::cout << "Cleared " << count << " monsters" << std::endl;
    }
}

void PrairieKing::SpawnDebugMonster(int type)
{
    // Calculate spawn position relative to player in game world coordinates
    Vector2 offset = {
        GetRandomFloat(-100, 100),
        GetRandomFloat(-100, 100)};

    // Ensure spawn position is within valid game bounds
    Vector2 spawnPos = {
        std::clamp(m_playerPosition.x + offset.x,
                   static_cast<float>(GetTileSize()),
                   static_cast<float>((MAP_WIDTH - 2) * GetTileSize())),
        std::clamp(m_playerPosition.y + offset.y,
                   static_cast<float>(GetTileSize()),
                   static_cast<float>((MAP_HEIGHT - 2) * GetTileSize()))};

    // Create monster with valid position
    CowboyMonster *monster = new CowboyMonster(m_assets, type, spawnPos);

    // Only add if spawn position is valid
    if (!IsCollidingWithMap(Rectangle{
            spawnPos.x,
            spawnPos.y,
            static_cast<float>(GetTileSize()),
            static_cast<float>(GetTileSize())}))
    {
        std::cout << "Spawning monster type " << type << " at ("
                  << spawnPos.x << ", " << spawnPos.y << ")" << std::endl;
        AddMonster(monster);
    }
    else
    {
        delete monster;
    }
}

void PrairieKing::SpawnDebugPowerup(int type)
{
    Vector2 spawnPos = {
        m_playerPosition.x + GetRandomFloat(-50, 50),
        m_playerPosition.y + GetRandomFloat(-50, 50)};

    m_powerups.push_back(CowboyPowerup(type, spawnPos, 10000));
}

void PrairieKing::DrawDebugGrid()
{
    const int tileSize = GetTileSize();

    // Draw grid
    for (int x = 0; x <= MAP_WIDTH; x++)
    {
        DrawLineV(
            Vector2{m_topLeftScreenCoordinate.x + x * tileSize, m_topLeftScreenCoordinate.y},
            Vector2{m_topLeftScreenCoordinate.x + x * tileSize, m_topLeftScreenCoordinate.y + MAP_HEIGHT * tileSize},
            ColorAlpha(GRAY, 0.3f));
    }

    for (int y = 0; y <= MAP_HEIGHT; y++)
    {
        DrawLineV(
            Vector2{m_topLeftScreenCoordinate.x, m_topLeftScreenCoordinate.y + y * tileSize},
            Vector2{m_topLeftScreenCoordinate.x + MAP_WIDTH * tileSize, m_topLeftScreenCoordinate.y + y * tileSize},
            ColorAlpha(GRAY, 0.3f));
    }
}

void PrairieKing::DrawDebugInfo()
{
    if (!m_debugMode)
        return;

    // Draw debug mode indicator
    DrawText("DEBUG MODE", 10, 10, 20, RED);

    // Draw debug grid
    DrawDebugGrid();

    const float startY = 40; // Moved down to not overlap with debug indicator
    float currentY = startY;
    const float lineHeight = 20;
    const Color debugColor = YELLOW;

    // Draw FPS
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, currentY, 20, debugColor);
    currentY += lineHeight;

    // Game State Info
    DrawText(TextFormat("Wave: %d/%d", m_whichWave + 1, 12), 10, currentY, 20, debugColor);
    currentY += lineHeight;

    DrawText(TextFormat("Wave Timer: %.1f", m_waveTimer / 1000.0f), 10, currentY, 20, debugColor);
    currentY += lineHeight;

    DrawText(TextFormat("Player Pos: (%.1f, %.1f)", m_playerPosition.x, m_playerPosition.y),
             10, currentY, 20, debugColor);
    currentY += lineHeight;

    // Entities count
    DrawText(TextFormat("Monsters: %d", m_monsters.size()), 10, currentY, 20, debugColor);
    currentY += lineHeight;

    DrawText(TextFormat("Bullets: %d", m_bullets.size()), 10, currentY, 20, debugColor);
    currentY += lineHeight;

    DrawText(TextFormat("Powerups: %d", m_powerups.size()), 10, currentY, 20, debugColor);
    currentY += lineHeight;

    // Player Stats
    DrawText(TextFormat("Lives: %d", m_lives), 10, currentY, 20, debugColor);
    currentY += lineHeight;

    DrawText(TextFormat("Damage: %d", m_bulletDamage), 10, currentY, 20, debugColor);
    currentY += lineHeight;

    // Active powerups
    currentY += lineHeight;
    DrawText("Active Powerups:", 10, currentY, 20, debugColor);
    currentY += lineHeight;
    for (const auto &powerup : m_activePowerups)
    {
        DrawText(TextFormat("Type %d: %.1fs", powerup.first, powerup.second / 1000.0f),
                 10, currentY, 20, debugColor);
        currentY += lineHeight;
    }

    // Debug Controls Help
    const float rightCol = GetScreenWidth() - 200;
    currentY = startY;
    DrawText("Debug Controls:", rightCol, currentY, 20, debugColor);
    currentY += lineHeight;
    DrawText("F3: Toggle Debug", rightCol, currentY, 20, debugColor);
    currentY += lineHeight;
    DrawText("F5: Add Life", rightCol, currentY, 20, debugColor);
    currentY += lineHeight;
    DrawText("F6: Add Coins", rightCol, currentY, 20, debugColor);
    currentY += lineHeight;
    DrawText("F7: Inc Damage", rightCol, currentY, 20, debugColor);
    currentY += lineHeight;
    DrawText("F8: Clear Monsters", rightCol, currentY, 20, debugColor);
    currentY += lineHeight;
    DrawText("Num1-7: Spawn Monster", rightCol, currentY, 20, debugColor);
    currentY += lineHeight;
    DrawText("Num8-9: Spawn Powerup", rightCol, currentY, 20, debugColor);

    // Draw monster hitboxes
    for (const auto &monster : m_monsters)
    {
        DrawRectangleLinesEx(
            Rectangle{
                m_topLeftScreenCoordinate.x + monster->position.x,
                m_topLeftScreenCoordinate.y + monster->position.y,
                monster->position.width,
                monster->position.height},
            1,
            RED);
    }

    // Draw player hitbox
    DrawRectangleLinesEx(
        Rectangle{
            m_topLeftScreenCoordinate.x + m_playerBoundingBox.x,
            m_topLeftScreenCoordinate.y + m_playerBoundingBox.y,
            m_playerBoundingBox.width,
            m_playerBoundingBox.height},
        1,
        GREEN);
}

void PrairieKing::ApplyPurchasedUpgrade(int itemId) {
    switch (itemId) {
        case ITEM_AMMO1:
        case ITEM_AMMO2:
        case ITEM_AMMO3:
            m_ammoLevel++;
            m_bulletDamage++;
            break;

        case ITEM_FIRESPEED1:
        case ITEM_FIRESPEED2:
        case ITEM_FIRESPEED3:
            m_fireSpeedLevel++;
            break;

        case ITEM_RUNSPEED1:
        case ITEM_RUNSPEED2:
            m_runSpeedLevel++;
            break;

        case ITEM_LIFE:
            m_lives++;
            break;

        case ITEM_SPREADPISTOL:
            m_spreadPistol = true;
            break;

        case ITEM_STAR:
            if (!m_heldItem) {
                m_heldItem = std::make_unique<CowboyPowerup>(POWERUP_SHERRIFF, 
                    Vector2{0, 0}, 9999);
            }
            break;
    }
}

void PrairieKing::DrawShopping(const Texture2D& texture, Vector2 topLeftScreenCoordinate) {
    // Draw arriving/leaving merchant animation
    if ((m_merchantArriving || m_merchantLeaving) && !m_merchantShopOpen) {
        DrawTexturePro(texture, 
            Rectangle{336.0f + ((m_shoppingTimer / 100 % 2 == 0) ? 16.0f : 0), 80, 16, 16},
            Rectangle{topLeftScreenCoordinate.x + m_merchantBox.x, 
                     topLeftScreenCoordinate.y + m_merchantBox.y, 
                     48, 48},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }
    else {
        // Draw merchant facing direction based on player position
        int whichFrame = (m_playerBoundingBox.x - m_merchantBox.x > GetTileSize()) ? 2 :
                        (m_merchantBox.x - m_playerBoundingBox.x > GetTileSize()) ? 1 : 0;
        
        // Draw merchant
        DrawTexturePro(texture,
            Rectangle{368.0f + whichFrame * 16, 80, 16, 16},
            Rectangle{topLeftScreenCoordinate.x + m_merchantBox.x,
                     topLeftScreenCoordinate.y + m_merchantBox.y,
                     48, 48}, 
            Vector2{0, 0},
            0.0f,
            WHITE);

        // Draw merchant shop counter
        DrawTexturePro(texture,
            Rectangle{401, 96, 63, 32},
            Rectangle{topLeftScreenCoordinate.x + m_merchantBox.x - GetTileSize(),
                     topLeftScreenCoordinate.y + m_merchantBox.y + GetTileSize(),
                     189, 96},
            Vector2{0, 0},
            0.0f,
            WHITE);

        // Draw store items
        for (const auto& item : m_storeItems) {
            // Draw item sprite
            DrawTexturePro(texture,
                Rectangle{192.0f + item.second * 16, 128, 16, 16},
                Rectangle{topLeftScreenCoordinate.x + item.first.x,
                         topLeftScreenCoordinate.y + item.first.y,
                         48, 48},
                Vector2{0, 0},
                0.0f,
                WHITE);

            // Draw item price
            std::string priceText = std::to_string(GetPriceForItem(item.second));
            Vector2 textSize = MeasureTextEx(m_assets.GetFont("small"), priceText.c_str(), 16, 1);
            
            Color priceColor = {88, 29, 43, 255};
            
            // Draw price with outline effect
            DrawTextEx(m_assets.GetFont("small"),
                priceText.c_str(),
                Vector2{topLeftScreenCoordinate.x + item.first.x + GetTileSize()/2 - textSize.x/2,
                       topLeftScreenCoordinate.y + item.first.y + GetTileSize() + 3},
                16,
                1,
                priceColor);

            DrawTextEx(m_assets.GetFont("small"), 
                priceText.c_str(),
                Vector2{topLeftScreenCoordinate.x + item.first.x + GetTileSize()/2 - textSize.x/2 - 1,
                       topLeftScreenCoordinate.y + item.first.y + GetTileSize() + 3},
                16,
                1,
                priceColor);

            DrawTextEx(m_assets.GetFont("small"),
                priceText.c_str(), 
                Vector2{topLeftScreenCoordinate.x + item.first.x + GetTileSize()/2 - textSize.x/2 + 1,
                       topLeftScreenCoordinate.y + item.first.y + GetTileSize() + 3},
                16,
                1,
                priceColor);
        }
    }

    // Draw down arrow when player can move to next level
    if (m_waitingForPlayerToMoveDownAMap && 
        (m_merchantShopOpen || m_merchantLeaving || !m_shopping) && 
        m_shoppingTimer < 250) {
            
        DrawTexturePro(texture,
            Rectangle{207, 102, 8, 8},
            Rectangle{topLeftScreenCoordinate.x + 8.5f * GetTileSize() - 12,
                     topLeftScreenCoordinate.y + 15.0f * GetTileSize(),
                     24, 24},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }
}

void PrairieKing::PauseScreen()
{
}