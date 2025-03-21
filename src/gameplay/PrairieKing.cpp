#include "gameplay/PrairieKing.hpp"

namespace {
    bool VectorsEqual(const Vector2& a, const Vector2& b) {
        return a.x == b.x && a.y == b.y;
    }
}

PrairieKing::PrairieKing(AssetManager& assets) 
    : m_assets(assets)
    , m_map(MAP_WIDTH, std::vector<int>(MAP_HEIGHT))
    , m_nextMap(MAP_WIDTH, std::vector<int>(MAP_HEIGHT))
{
    Initialize();
}

PrairieKing::~PrairieKing() = default;

void PrairieKing::Initialize() {
    // Initialize screen position
    m_topLeftScreenCoordinate = {
        static_cast<float>(GetScreenWidth() / 2 - MAP_WIDTH * TILE_SIZE / 2),
        static_cast<float>(GetScreenHeight() / 2 - MAP_HEIGHT * TILE_SIZE / 2)
    };
    
    // Initialize game state
    m_died = false;
    m_onStartMenu = true;
    m_gameOver = false;
    m_shopping = false;
    m_merchantShopOpen = false;
    m_waitingForPlayerToMoveDownAMap = false;
    m_scrollingMap = false;
    m_hasGopherAppeared = false;
    m_shootoutLevel = false;
    m_gopherTrain = false;
    m_playerJumped = false;
    m_endCutscene = false;

    // Initialize stats
    m_lives = 3;
    m_coins = 0;
    m_score = 0;
    m_bulletDamage = 1;
    m_whichWave = 0;
    m_waveTimer = WAVE_DURATION;
    m_betweenWaveTimer = BETWEEN_WAVE_DURATION;
    m_currentWorld = World::Desert;

    // Initialize player
    m_playerPosition = { 
        static_cast<float>(MAP_WIDTH * TILE_SIZE / 2),
        static_cast<float>(MAP_HEIGHT * TILE_SIZE / 2)
    };
    m_playerBoundingBox = {
        m_playerPosition.x + TILE_SIZE / 4,
        m_playerPosition.y + TILE_SIZE / 4,
        static_cast<float>(TILE_SIZE / 2),
        static_cast<float>(TILE_SIZE / 2)
    };
    m_playerMotionAnimationTimer = 0;
    m_playerFootstepSoundTimer = PLAYER_FOOTSTEP_DELAY;
    m_playerInvincibleTimer = 0;
    m_deathTimer = 0;

    // Initialize input tracking
    m_buttonHeldFrames.clear();
    for (int i = 0; i < static_cast<int>(GameKeys::MAX); ++i) {
        m_buttonHeldFrames[static_cast<GameKeys>(i)] = 0;
    }

    // Initialize first map
    InitializeMap();

    // Initialize new state variables
    m_isTransitioningToGameOver = false;
    m_gameOverTransitionTimer = 0;
    m_fadeOutAlpha = 0;

    // Initialize monster spawn chances
    m_monsterChances = {
        {0.014f, 0.4f},  // Orc
        {0.0f, 0.0f},    // Ghost 
        {0.0f, 0.0f},    // Ogre
        {0.0f, 0.0f},    // Mummy
        {0.0f, 0.0f},    // Devil
        {0.0f, 0.0f},    // Mushroom
        {0.0f, 0.0f}     // Spikey
    };

    // Remove player 2 initialization
    m_playingWithAbigail = false;
    m_beatLevelWithAbigail = false;
    m_player2Position = {0, 0};
    m_player2BoundingBox = {0, 0, 0, 0};
    
    // Initialize music state
    if (m_overworldSong.frameCount == 0) {
        m_overworldSong = m_assets.GetMusic("overworld");
        m_outlawSong = m_assets.GetMusic("outlaw"); 
        m_zombieSong = m_assets.GetMusic("zombie");
    }
}

void PrairieKing::Update(float deltaTime) {
    if (m_isTransitioningToGameOver) {
        UpdateGameOverTransition(deltaTime);
        return;
    }

    // Update input state
    m_buttonHeldState.clear();
    UpdateInput();
    
    // Update button held frames
    for (int i = 0; i < static_cast<int>(GameKeys::MAX); i++) {
        GameKeys key = static_cast<GameKeys>(i);
        if (m_buttonHeldState.find(key) != m_buttonHeldState.end()) {
            m_buttonHeldFrames[key]++;
        } else {
            m_buttonHeldFrames[key] = 0;
        }
    }

    if (m_onStartMenu) {
        UpdateStartMenu(deltaTime);
        return;
    }

    if (m_gameOver) {
        UpdateGameOver(deltaTime);
        return;
    }

    // Update game timers
    if (m_deathTimer > 0) {
        m_deathTimer -= deltaTime * 1000.0f;
    }
    
    if (m_playerInvincibleTimer > 0) {
        m_playerInvincibleTimer -= deltaTime * 1000.0f;
    }

    // Update wave timers if not in special states
    if (!m_shopping && !m_waitingForPlayerToMoveDownAMap && !m_shootoutLevel) {
        if (m_betweenWaveTimer > 0) {
            m_betweenWaveTimer -= deltaTime * 1000.0f;
        } else if (m_waveTimer > 0) {
            m_waveTimer -= deltaTime * 1000.0f;
        }
    }

    // Update player movement and shooting if not dead
    if (m_deathTimer <= 0) {
        UpdatePlayerMovement(deltaTime);
        UpdatePlayerShooting(deltaTime);
    }

    // Update game entities
    UpdateBullets(deltaTime);
    UpdateMonsters(deltaTime);
    UpdatePowerups(deltaTime);
    UpdateWave(deltaTime);

    // Update music streams
    if (m_waveTimer > 0 && m_betweenWaveTimer <= 0 && m_zombieModeTimer <= 0 && !m_shootoutLevel) {
        if (!IsMusicStreamPlaying(m_overworldSong)) {
            PlayMusicStream(m_overworldSong);
        }
        UpdateMusicStream(m_overworldSong);
    }
    
    if (m_shootoutLevel) {
        if (!IsMusicStreamPlaying(m_outlawSong)) {
            StopMusicStream(m_overworldSong);
            PlayMusicStream(m_outlawSong);
        }
        UpdateMusicStream(m_outlawSong);
    }
    
    if (m_zombieModeTimer > 0) {
        if (!IsMusicStreamPlaying(m_zombieSong)) {
            StopMusicStream(m_overworldSong);
            PlayMusicStream(m_zombieSong);
        }
        UpdateMusicStream(m_zombieSong);
    }
}

void PrairieKing::UpdateInput() {
    // Keyboard input
    if (IsKeyDown(KEY_W)) m_buttonHeldState.insert(GameKeys::MoveUp);
    if (IsKeyDown(KEY_S)) m_buttonHeldState.insert(GameKeys::MoveDown);
    if (IsKeyDown(KEY_A)) m_buttonHeldState.insert(GameKeys::MoveLeft);
    if (IsKeyDown(KEY_D)) m_buttonHeldState.insert(GameKeys::MoveRight);
    
    if (IsKeyDown(KEY_UP)) m_buttonHeldState.insert(GameKeys::ShootUp);
    if (IsKeyDown(KEY_DOWN)) m_buttonHeldState.insert(GameKeys::ShootDown);
    if (IsKeyDown(KEY_LEFT)) m_buttonHeldState.insert(GameKeys::ShootLeft);
    if (IsKeyDown(KEY_RIGHT)) m_buttonHeldState.insert(GameKeys::ShootRight);
    
    if (IsKeyDown(KEY_SPACE)) m_buttonHeldState.insert(GameKeys::UsePowerup);
    if (IsKeyDown(KEY_ENTER)) m_buttonHeldState.insert(GameKeys::SelectOption);
    if (IsKeyDown(KEY_ESCAPE)) m_buttonHeldState.insert(GameKeys::Exit);
}

void PrairieKing::UpdateStartMenu(float deltaTime) {
    // Start menu logic similar to original game
    if (m_buttonHeldState.size() > 0 || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_onStartMenu = false;
        m_betweenWaveTimer = BETWEEN_WAVE_DURATION;
        // Play start sound
        PlaySound(m_assets.GetSound("pickup_coin"));
    }
}

void PrairieKing::UpdateGameOver(float deltaTime) {
    // Handle game over menu navigation and selection
    if (m_buttonHeldFrames[GameKeys::MoveUp] == 1) {
        m_gameOverOption = std::max(0, m_gameOverOption - 1);
        PlaySound(m_assets.GetSound("gunshot"));
    } else if (m_buttonHeldFrames[GameKeys::MoveDown] == 1) {
        m_gameOverOption = std::min(1, m_gameOverOption + 1);
        PlaySound(m_assets.GetSound("gunshot"));
    }

    if (m_buttonHeldFrames[GameKeys::SelectOption] == 1) {
        if (m_gameOverOption == 0) {
            // Restart game
            Initialize();
            PlaySound(m_assets.GetSound("pickup_coin"));
        } else {
            // Quit game
            m_shouldQuit = true;
        }
    }
}

void PrairieKing::Draw() {
    // Draw map tiles
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            Rectangle sourceRect;
            switch (static_cast<MapTile>(m_map[x][y])) {
                case MapTile::Barrier1: sourceRect = {224, 1728, 16, 16}; break;
                case MapTile::Barrier2: sourceRect = {240, 1728, 16, 16}; break;
                case MapTile::Rocky1:   sourceRect = {256, 1728, 16, 16}; break;
                case MapTile::Desert:   sourceRect = {272, 1728, 16, 16}; break;
                case MapTile::Grassy:   sourceRect = {288, 1728, 16, 16}; break;
                case MapTile::Cactus:   sourceRect = {304, 1728, 16, 16}; break;
                case MapTile::Fence:    sourceRect = {336, 1728, 16, 16}; break;
                case MapTile::Trench1:  sourceRect = {352, 1728, 16, 16}; break;
                case MapTile::Trench2:  sourceRect = {368, 1728, 16, 16}; break;
                case MapTile::Bridge:   sourceRect = {384, 1728, 16, 16}; break;
            }
            
            DrawTextureRec(m_assets.GetTexture("cursors"),
                sourceRect,
                {m_topLeftScreenCoordinate.x + x * TILE_SIZE,
                 m_topLeftScreenCoordinate.y + y * TILE_SIZE},
                WHITE);
        }
    }

    // Draw monsters
    for (const auto& monster : m_monsters) {
        monster->Draw(m_topLeftScreenCoordinate, m_assets.GetTexture("cursors"));
    }

    // Draw powerups
    for (const auto& powerup : m_powerups) {
        powerup->Draw(m_topLeftScreenCoordinate, m_assets.GetTexture("cursors"));
    }

    // Draw bullets
    for (const auto& bullet : m_bullets) {
        bullet->Draw(m_topLeftScreenCoordinate, m_assets.GetTexture("cursors"));
    }
    for (const auto& bullet : m_enemyBullets) {
        bullet->Draw(m_topLeftScreenCoordinate, m_assets.GetTexture("cursors"));
    }

    // Draw player
    if (m_deathTimer <= 0 && (m_playerInvincibleTimer <= 0 || m_playerInvincibleTimer / 100 % 2 == 0)) {
        Rectangle playerSourceRect = {
            static_cast<float>(384 + (static_cast<int>(movementAnimationTimer / 100) % 4) * 16), 
            static_cast<float>(1760),
            16,
            16
        };
        
        DrawTextureRec(m_assets.GetTexture("cursors"),
            playerSourceRect,
            {m_topLeftScreenCoordinate.x + m_playerPosition.x,
             m_topLeftScreenCoordinate.y + m_playerPosition.y},
            WHITE);
    }

    // Draw UI
    DrawText(TextFormat("LIVES: %d", m_lives), 
        m_topLeftScreenCoordinate.x + 10, 
        m_topLeftScreenCoordinate.y + MAP_HEIGHT * TILE_SIZE + 10,
        20, WHITE);
        
    DrawText(TextFormat("SCORE: %d", m_score),
        m_topLeftScreenCoordinate.x + MAP_WIDTH * TILE_SIZE - 150,
        m_topLeftScreenCoordinate.y + MAP_HEIGHT * TILE_SIZE + 10,
        20, WHITE);

    if (m_heldItem) {
        Rectangle sourceRect = {272 + static_cast<int>(m_heldItem->GetType()) * 16, 1808, 16, 16};
        DrawTextureRec(m_assets.GetTexture("cursors"),
            sourceRect,
            {m_topLeftScreenCoordinate.x + 10,
             m_topLeftScreenCoordinate.y + 10},
            WHITE);
    }

    if (m_isTransitioningToGameOver) {
        DrawTransitionEffects();
    }
}

bool PrairieKing::IsGameOver() const {
    // We'll implement this later
    return false;
}

bool PrairieKing::InitializeWave(int wave) {
    // Clear the map first
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            m_map[x][y] = static_cast<int>(MapTile::Desert);
        }
    }

    switch (wave) {
        case 1:
            // First wave setup - basic barriers
            AddBarrier(4, 4, 7);
            AddBarrier(4, 5, 7);
            AddBarrier(5, 4, 7);
            AddBarrier(12, 4, 7);
            AddBarrier(11, 4, 7);
            AddBarrier(12, 5, 7);
            AddBarrier(4, 12, 7);
            AddBarrier(5, 12, 7);
            AddBarrier(4, 11, 7);
            AddBarrier(12, 12, 7);
            AddBarrier(11, 12, 7);
            AddBarrier(12, 11, 7);
            break;

        case 2:
            // Second wave - cross pattern
            AddBarrier(8, 4, 7);
            AddBarrier(12, 8, 7);
            AddBarrier(8, 12, 7);
            AddBarrier(4, 8, 7);
            AddCactus(1, 1);
            AddCactus(14, 1);
            AddCactus(14, 14);
            AddCactus(1, 14);
            break;

        // Add more wave patterns as needed
        default:
            // Default wave setup - corner barriers
            AddCactus(4, 4);
            AddCactus(12, 4);
            AddCactus(4, 12);
            AddCactus(12, 12);
            break;
    }

    return true;
}

void PrairieKing::UpdateWave(float deltaTime) {
    if (m_waveTimer <= 0 && m_monsters.empty() && m_betweenWaveTimer <= 0) {
        StartNewWave();
        return;
    }

    // Spawn new monsters based on wave timer and monster chances
    if (m_waveTimer > 0 && !m_shopping && !m_waitingForPlayerToMoveDownAMap) {
        for (size_t i = 0; i < m_monsterChances.size(); i++) {
            if (GetRandomValue(0, 100) < m_monsterChances[i].x * 100.0f) {
                while (GetRandomValue(0, 100) < m_monsterChances[i].y * 100.0f && 
                       m_monsters.size() < 15) {
                    SpawnMonster(static_cast<MonsterType>(i));
                }
            }
        }
    }
}

void PrairieKing::StartNewWave() {
    m_whichWave++;
    m_waveTimer = WAVE_DURATION;
    m_betweenWaveTimer = BETWEEN_WAVE_DURATION;

    // Update monster spawn chances based on wave number
    UpdateMonsterChances();
    
    // Initialize new wave map
    InitializeWave(m_whichWave);

    // Special wave handling
    if (m_whichWave % 4 == 0) {
        StartShoppingLevel();
    } else if (m_whichWave > 0) {
        m_waitingForPlayerToMoveDownAMap = true;
    }
}

bool PrairieKing::IsCollidingWithMap(const Rectangle& rect) const {
    // Check map boundaries
    if (rect.x < 0 || rect.y < 0 || 
        rect.x >= MAP_WIDTH * TILE_SIZE || 
        rect.y >= MAP_HEIGHT * TILE_SIZE) {
        return true;
    }

    // Check each corner of the rectangle against map tiles
    Vector2 corners[] = {
        {rect.x, rect.y},
        {rect.x + rect.width, rect.y},
        {rect.x, rect.y + rect.height},
        {rect.x + rect.width, rect.y + rect.height}
    };

    for (const auto& corner : corners) {
        int tileX = static_cast<int>(corner.x) / TILE_SIZE;
        int tileY = static_cast<int>(corner.y) / TILE_SIZE;
        
        if (tileX < 0 || tileY < 0 || tileX >= MAP_WIDTH || tileY >= MAP_HEIGHT) {
            continue;
        }

        MapTile tile = static_cast<MapTile>(m_map[tileX][tileY]);
        if (tile == MapTile::Barrier1 || tile == MapTile::Barrier2 || 
            tile == MapTile::Cactus || tile == MapTile::Fence) {
            return true;
        }
    }

    return false;
}

void PrairieKing::UpdatePlayerMovement(float deltaTime) {
    std::vector<GameKeys> movementKeys;
    if (m_buttonHeldFrames[GameKeys::MoveLeft] > 0) movementKeys.push_back(GameKeys::MoveLeft);
    if (m_buttonHeldFrames[GameKeys::MoveRight] > 0) movementKeys.push_back(GameKeys::MoveRight);
    if (m_buttonHeldFrames[GameKeys::MoveUp] > 0) movementKeys.push_back(GameKeys::MoveUp);
    if (m_buttonHeldFrames[GameKeys::MoveDown] > 0) movementKeys.push_back(GameKeys::MoveDown);

    if (movementKeys.empty() || m_gopherTrain) return;

    // Calculate base movement speed
    float speed = 3.0f;
    if (movementKeys.size() > 1) {
        speed = sqrt(2.0f * (speed * speed)) / 2.0f;
    }

    // Apply speed modifiers
    if (m_activePowerups.find(PowerupType::Speed) != m_activePowerups.end()) {
        speed *= 1.5f;
    }
    if (m_zombieModeTimer > 0) {
        speed *= 1.5f;
    }
    speed *= (1.0f + m_runSpeedLevel * 0.25f);

    // Store old position for collision check
    Rectangle newPlayerBox = m_playerBoundingBox;

    // Apply movement
    for (GameKeys key : movementKeys) {
        switch (key) {
            case GameKeys::MoveUp:
                newPlayerBox.y -= static_cast<float>(speed);
                break;
            case GameKeys::MoveDown:
                newPlayerBox.y += static_cast<float>(speed);
                break;
            case GameKeys::MoveLeft:
                newPlayerBox.x -= static_cast<float>(speed);
                break;
            case GameKeys::MoveRight:
                newPlayerBox.x += static_cast<float>(speed);
                break;
            default:
                break;
        }
    }

    // Check collisions and update position if valid
    if (!IsCollidingWithMap(newPlayerBox)) {
        m_playerBoundingBox = newPlayerBox;
        m_playerPosition.x = m_playerBoundingBox.x - TILE_SIZE / 4;
        m_playerPosition.y = m_playerBoundingBox.y - TILE_SIZE / 4;

        // Update animation
        m_playerMotionAnimationTimer += deltaTime * 1000.0f;
        m_playerMotionAnimationTimer = fmodf(m_playerMotionAnimationTimer, 400.0f);

        // Play footstep sound
        m_playerFootstepSoundTimer -= deltaTime * 1000.0f;
        if (m_playerFootstepSoundTimer <= 0) {
            PlaySound(m_assets.GetSound("cowboy_footstep"));
            m_playerFootstepSoundTimer = PLAYER_FOOTSTEP_DELAY;
        }
    }
}

void PrairieKing::UpdatePlayerShooting(float deltaTime) {
    if (m_deathTimer > 0) return;

    std::vector<GameKeys> shootingKeys;
    if (m_buttonHeldFrames[GameKeys::ShootLeft] > 0) shootingKeys.push_back(GameKeys::ShootLeft);
    if (m_buttonHeldFrames[GameKeys::ShootRight] > 0) shootingKeys.push_back(GameKeys::ShootRight);
    if (m_buttonHeldFrames[GameKeys::ShootUp] > 0) shootingKeys.push_back(GameKeys::ShootUp);
    if (m_buttonHeldFrames[GameKeys::ShootDown] > 0) shootingKeys.push_back(GameKeys::ShootDown);

    if (shootingKeys.empty() || m_shotTimer > 0) return;

    // Calculate shooting position
    Vector2 shootPos = {
        m_playerPosition.x + TILE_SIZE / 2,
        m_playerPosition.y + TILE_SIZE / 2
    };

    // Handle different shooting patterns
    if (m_activePowerups.find(PowerupType::Spread) != m_activePowerups.end()) {
        // Spread shot pattern
        for (int dir = 0; dir < 4; dir++) {
            SpawnBullet(shootPos, dir);
            SpawnBullet(shootPos, dir, 0.25f);
            SpawnBullet(shootPos, dir, -0.25f);
        }
    } else {
        // Normal shooting
        for (GameKeys key : shootingKeys) {
            int direction = -1;
            switch (key) {
                case GameKeys::ShootUp: direction = 0; break;
                case GameKeys::ShootRight: direction = 1; break;
                case GameKeys::ShootDown: direction = 2; break;
                case GameKeys::ShootLeft: direction = 3; break;
                default: break;
            }
            if (direction != -1) {
                SpawnBullet(shootPos, direction);
            }
        }
    }

    // Play sound and set cooldown
    PlaySound(m_assets.GetSound("cowboy_gunshot"));
    m_shotTimer = m_shootingDelay;
    
    // Apply rapid fire modifier
    if (m_activePowerups.find(PowerupType::RapidFire) != m_activePowerups.end()) {
        m_shotTimer /= 4;
    }
    m_shotTimer *= (1.0f - m_fireSpeedLevel * 0.15f);
    
    // Minimum shot timer
    m_shotTimer = std::max(m_shotTimer, 20);
}

void PrairieKing::SpawnBullet(const Vector2& position, int direction, float angleOffset) {
    Vector2 bulletPos = position;
    Vector2 bulletMotion = {0, 0};
    
    float angle = direction * PI/2 + angleOffset;
    bulletMotion.x = cosf(angle) * BULLET_SPEED;
    bulletMotion.y = -sinf(angle) * BULLET_SPEED;

    m_bullets.push_back(std::make_unique<CowboyBullet>(bulletPos, bulletMotion, m_bulletDamage));
}

void PrairieKing::UpdateMonsterChances() {
    switch (m_whichWave) {
        case 1:
        case 2:
        case 3:
            m_monsterChances[0] = {m_monsterChances[0].x + 0.001f, m_monsterChances[0].y + 0.02f};  // Orc
            if (m_whichWave > 1) {
                m_monsterChances[2] = {m_monsterChances[2].x + 0.001f, m_monsterChances[2].y + 0.01f};  // Ogre
            }
            m_monsterChances[6] = {m_monsterChances[6].x + 0.001f, m_monsterChances[6].y + 0.01f};  // Spikey
            break;

        case 4:
        case 5:
        case 6:
        case 7:
            if (VectorsEqual(m_monsterChances[5], {0, 0})) {
                m_monsterChances[5] = {0.01f, 0.15f};  // Mushroom initial spawn
            }
            m_monsterChances[0] = {0, 0};  // Disable Orcs
            m_monsterChances[6] = {0, 0};  // Disable Spikeys
            m_monsterChances[2] = {m_monsterChances[2].x + 0.002f, m_monsterChances[2].y + 0.02f};  // Ogres
            m_monsterChances[5] = {m_monsterChances[5].x + 0.001f, m_monsterChances[5].y + 0.02f};  // Mushrooms
            m_monsterChances[1] = {m_monsterChances[1].x + 0.0018f, m_monsterChances[1].y + 0.08f};  // Ghosts
            break;

        case 8:
        case 9:
        case 10:
        case 11:
            m_monsterChances[5] = {0, 0};  // Disable Mushrooms
            m_monsterChances[1] = {0, 0};  // Disable Ghosts
            m_monsterChances[2] = {0, 0};  // Disable Ogres
            
            if (VectorsEqual(m_monsterChances[3], {0, 0})) {
                m_monsterChances[3] = {0.012f, 0.4f};  // Mummy initial spawn
            }
            if (VectorsEqual(m_monsterChances[4], {0, 0})) {
                m_monsterChances[4] = {0.003f, 0.1f};  // Devil initial spawn
            }
            
            m_monsterChances[3] = {m_monsterChances[3].x + 0.002f, m_monsterChances[3].y + 0.05f};  // Mummies
            m_monsterChances[4] = {m_monsterChances[4].x + 0.0015f, m_monsterChances[4].y + 0.04f};  // Devils
            
            if (m_whichWave == 11) {
                m_monsterChances[4] = {m_monsterChances[4].x + 0.01f, m_monsterChances[4].y + 0.04f};  // Devils
                m_monsterChances[3] = {m_monsterChances[3].x - 0.01f, m_monsterChances[3].y + 0.04f};  // Mummies
            }
            break;
    }
}

void PrairieKing::UpdateMonsters(float deltaTime) {
    // Update existing monsters
    for (auto it = m_monsters.begin(); it != m_monsters.end();) {
        auto& monster = *it;
        
        // Check collisions with player
        if (CheckCollisionRecs(monster->GetBounds(), m_playerBoundingBox) && 
            m_playerInvincibleTimer <= 0) {
            PlayerDie();
            break;
        }
        
        // Check collisions with bullets
        bool monsterDied = false;
        for (auto bulletIt = m_bullets.begin(); bulletIt != m_bullets.end();) {
            if (monster->IsCollidingWith((*bulletIt)->GetBounds())) {
                if (monster->TakeDamage((*bulletIt)->GetDamage())) {
                    // Monster died
                    monsterDied = true;
                    SpawnLoot(monster->GetPosition(), monster->GetLootDrop());
                }
                bulletIt = m_bullets.erase(bulletIt);
            } else {
                ++bulletIt;
            }
        }
        
        if (monsterDied) {
            it = m_monsters.erase(it);
        } else {
            monster->Update(deltaTime, m_playerPosition);
            ++it;
        }
    }
}

void PrairieKing::SpawnLoot(const Vector2& position, int lootType) {
    if (lootType >= 0) {
        m_powerups.push_back(std::make_unique<CowboyPowerup>(
            lootType,
            position,
            LOOT_DURATION
        ));
    }
}

void PrairieKing::UpdateBullets(float deltaTime) {
    // Update player bullets
    for (auto it = m_bullets.begin(); it != m_bullets.end();) {
        auto& bullet = *it;
        bullet->Update();
        
        // Check if bullet is out of bounds
        Vector2 pos = bullet->GetPosition();
        if (pos.x < 0 || pos.y < 0 || 
            pos.x >= MAP_WIDTH * TILE_SIZE || 
            pos.y >= MAP_HEIGHT * TILE_SIZE) {
            it = m_bullets.erase(it);
            continue;
        }
        
        // Check collision with map
        int tileX = static_cast<int>(pos.x + 6) / TILE_SIZE;
        int tileY = static_cast<int>(pos.y + 6) / TILE_SIZE;
        if (static_cast<MapTile>(m_map[tileX][tileY]) == MapTile::Fence) {
            it = m_bullets.erase(it);
            continue;
        }
        
        ++it;
    }

    // Update enemy bullets
    for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end();) {
        auto& bullet = *it;
        bullet->Update();
        
        // Check if bullet is out of bounds
        Vector2 pos = bullet->GetPosition();
        if (pos.x < 0 || pos.y < 0 || 
            pos.x >= MAP_WIDTH * TILE_SIZE || 
            pos.y >= MAP_HEIGHT * TILE_SIZE) {
            it = m_enemyBullets.erase(it);
            continue;
        }
        
        // Check collision with map
        int tileX = static_cast<int>(pos.x + 6) / TILE_SIZE;
        int tileY = static_cast<int>(pos.y + 6) / TILE_SIZE;
        if (static_cast<MapTile>(m_map[tileX][tileY]) == MapTile::Fence) {
            it = m_enemyBullets.erase(it);
            continue;
        }
        
        // Check collision with player
        if (m_playerInvincibleTimer <= 0 && m_deathTimer <= 0 &&
            CheckCollisionRecs(m_playerBoundingBox, bullet->GetBounds())) {
            PlayerDie();
            break;
        }
        
        ++it;
    }
}

void PrairieKing::UpdatePowerups(float deltaTime) {
    // Update active powerup timers
    for (auto it = m_activePowerups.begin(); it != m_activePowerups.end();) {
        it->second -= static_cast<int>(deltaTime * 1000.0f);
        if (it->second <= 0) {
            it = m_activePowerups.erase(it);
        } else {
            ++it;
        }
    }

    // Update powerup items in the world
    for (auto it = m_powerups.begin(); it != m_powerups.end();) {
        auto& powerup = *it;
        powerup->Update(deltaTime);
        
        // Check if powerup has expired
        if (powerup->GetDuration() <= 0) {
            it = m_powerups.erase(it);
            continue;
        }
        
        // Check if player collects powerup
        if (CheckCollisionRecs(m_playerBoundingBox, powerup->GetBounds())) {
            CollectPowerup(powerup.get());
            it = m_powerups.erase(it);
            continue;
        }
        
        ++it;
    }
}

void PrairieKing::PlayerDie() {
    m_gopherRunning = false;
    m_hasGopherAppeared = false;
    m_enemyBullets.clear();
    
    if (!m_shootoutLevel) {
        m_powerups.clear();
        m_monsters.clear();
    }
    
    m_died = true;
    m_activePowerups.clear();
    m_deathTimer = DEATH_DELAY;
    
    // Play death sound
    PlaySound(m_assets.GetSound("cowboy_dead"));
    
    // Add death animation
    // TODO: Add temporary sprite system for death animation
    
    m_waveTimer = std::min(WAVE_DURATION, m_waveTimer + 10000);
    m_betweenWaveTimer = 4000;
    m_lives--;
    m_playerInvincibleTimer = 5000;
    
    if (m_shootoutLevel) {
        m_playerPosition = {8 * TILE_SIZE, 3 * TILE_SIZE};
    } else {
        m_playerPosition = {8 * TILE_SIZE - TILE_SIZE, 8 * TILE_SIZE};
        UpdatePlayerBoundingBox();
    }
    
    if (m_lives < 0) {
        m_deathTimer *= 3.0f;
        TransitionToGameOver();
    }

    // Stop any playing music
    StopMusicStream(m_overworldSong);
    StopMusicStream(m_outlawSong);
    StopMusicStream(m_zombieSong);
}

void PrairieKing::TransitionToGameOver() {
    m_isTransitioningToGameOver = true;
    m_gameOverTransitionTimer = 2.0f;
    m_fadeOutAlpha = 0;
}

void PrairieKing::UpdateGameOverTransition(float deltaTime) {
    m_gameOverTransitionTimer -= deltaTime;
    m_fadeOutAlpha = (2.0f - m_gameOverTransitionTimer) / 2.0f;
    
    if (m_gameOverTransitionTimer <= 0) {
        m_gameOver = true;
        m_isTransitioningToGameOver = false;
    }
}

void PrairieKing::DrawTransitionEffects() {
    Color fadeColor = {0, 0, 0, static_cast<unsigned char>(m_fadeOutAlpha * 255)};
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), fadeColor);
}

void PrairieKing::CollectPowerup(CowboyPowerup* powerup) {
    switch (static_cast<PowerupType>(powerup->GetType())) {
        case PowerupType::Heart:
        case PowerupType::Skull:
        case PowerupType::Log:
            UsePowerup(powerup->GetType());
            break;
            
        case PowerupType::Coin1:
            m_coins++;
            PlaySound(m_assets.GetSound("pickup_coin"));
            break;
            
        case PowerupType::Coin5:
            m_coins += 5;
            PlaySound(m_assets.GetSound("pickup_coin"));
            break;
            
        case PowerupType::Life:
            m_lives++;
            PlaySound(m_assets.GetSound("cowboy_powerup"));
            break;
            
        default:
            if (!m_heldItem) {
                m_heldItem = std::make_unique<CowboyPowerup>(*powerup);
            } else {
                // Swap current held item with new one
                m_powerups.push_back(std::make_unique<CowboyPowerup>(*m_heldItem));
                m_heldItem = std::make_unique<CowboyPowerup>(*powerup);
            }
            PlaySound(m_assets.GetSound("cowboy_powerup"));
            break;
    }
}

void PrairieKing::InitializeMap() {
    // Initialize default map
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            m_map[x][y] = static_cast<int>(MapTile::Desert);
        }
    }

    // Add default barriers
    AddBarrier(4, 4, 7);
    AddBarrier(12, 4, 7);
    AddBarrier(4, 12, 7);
    AddBarrier(12, 12, 7);
}

void PrairieKing::AddBarrier(int x, int y, int type) {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        m_map[x][y] = type;
    }
}

void PrairieKing::AddCactus(int x, int y) {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        m_map[x][y] = static_cast<int>(MapTile::Cactus);
    }
}

void PrairieKing::UpdatePlayerBoundingBox() {
    m_playerBoundingBox = {
        m_playerPosition.x + TILE_SIZE / 4,
        m_playerPosition.y + TILE_SIZE / 4,
        static_cast<float>(TILE_SIZE / 2),
        static_cast<float>(TILE_SIZE / 2)
    };
}

void PrairieKing::StartShoppingLevel() {
    // Initialize shopping state
    m_shopping = true;
    m_merchantShopOpen = false;
    m_waitingForPlayerToMoveDownAMap = true;
    m_monsters.clear();
}

void PrairieKing::UsePowerup(int powerupType) {
    auto type = static_cast<PowerupType>(powerupType);
    
    // Handle powerup effects
    switch (type) {
        case PowerupType::Speed:
            m_activePowerups[type] = POWERUP_DURATION;
            break;
            
        case PowerupType::RapidFire:
            m_activePowerups[type] = POWERUP_DURATION;
            m_shotTimer = 0;
            break;
            
        case PowerupType::Life:
            m_lives++;
            PlaySound(m_assets.GetSound("cowboy_powerup"));
            break;
            
        case PowerupType::Spread:
            m_activePowerups[type] = POWERUP_DURATION;
            break;
            
        case PowerupType::Nuke:
            // Clear all monsters
            m_monsters.clear();
            PlaySound(m_assets.GetSound("cowboy_explosion"));
            break;
            
        default:
            break;
    }
}

// CowboyBullet implementation
PrairieKing::CowboyBullet::CowboyBullet(const Vector2& pos, const Vector2& mot, int dmg)
    : position(pos), motion(mot), damage(dmg) {}

void PrairieKing::CowboyBullet::Update() {
    position.x += motion.x;
    position.y += motion.y;
}

void PrairieKing::CowboyBullet::Draw(const Vector2& screenOffset, Texture2D texture) {
    DrawTextureRec(texture,
        Rectangle{448, 1776, 4, 4},
        Vector2{screenOffset.x + position.x, screenOffset.y + position.y},
        WHITE);
}

void PrairieKing::SpawnMonster(MonsterType type) {
    // Get random spawn position on map edges
    Vector2 spawnPos;
    int side = GetRandomValue(0, 3);  // 0=top, 1=right, 2=bottom, 3=left
    
    switch(side) {
        case 0: // Top
            spawnPos = {
                static_cast<float>(GetRandomValue(1, MAP_WIDTH-2) * TILE_SIZE),
                static_cast<float>(TILE_SIZE)
            };
            break;
        case 1: // Right
            spawnPos = {
                static_cast<float>((MAP_WIDTH-2) * TILE_SIZE),
                static_cast<float>(GetRandomValue(1, MAP_HEIGHT-2) * TILE_SIZE)
            };
            break;
        case 2: // Bottom
            spawnPos = {
                static_cast<float>(GetRandomValue(1, MAP_WIDTH-2) * TILE_SIZE),
                static_cast<float>((MAP_HEIGHT-2) * TILE_SIZE)
            };
            break;
        case 3: // Left
            spawnPos = {
                static_cast<float>(TILE_SIZE),
                static_cast<float>(GetRandomValue(1, MAP_HEIGHT-2) * TILE_SIZE)
            };
            break;
    }

    // Check if spawn position is valid
    Rectangle spawnRect = {
        spawnPos.x, 
        spawnPos.y, 
        static_cast<float>(TILE_SIZE), 
        static_cast<float>(TILE_SIZE)
    };
    
    // Don't spawn if colliding with something
    for (const auto& monster : m_monsters) {
        if (CheckCollisionRecs(spawnRect, monster->GetBounds())) {
            return;
        }
    }

    // Create monster based on type
    switch(type) {
        case MonsterType::Orc:
            m_monsters.push_back(std::make_unique<CowboyMonster>(type, spawnPos));
            break;
        case MonsterType::Ghost:
            m_monsters.push_back(std::make_unique<CowboyMonster>(type, spawnPos));
            m_monsters.back()->flyer = true;
            break;
        case MonsterType::Dracula:
            m_monsters.push_back(std::make_unique<Dracula>(this));
            break;
        default:
            m_monsters.push_back(std::make_unique<CowboyMonster>(type, spawnPos));
            break;
    }
}

// Add CowboyMonster implementation
class CowboyMonster {
protected:
    friend class PrairieKing;  // Add this line to CowboyMonster class in hpp file
    // ...existing code...
};

PrairieKing::CowboyMonster::CowboyMonster(MonsterType monsterType, const Vector2& position) {
    type = monsterType;
    bounds = {
        position.x,
        position.y,
        static_cast<float>(TILE_SIZE),
        static_cast<float>(TILE_SIZE)
    };
    
    // Set monster properties based on type
    switch(type) {
        case MonsterType::Orc:
            health = ORC_HEALTH;
            speed = ORC_SPEED;
            break;
        case MonsterType::Ghost:
            health = GHOST_HEALTH;
            speed = GHOST_SPEED;
            flyer = true;
            break;
        case MonsterType::Ogre:
            health = OGRE_HEALTH;
            speed = OGRE_SPEED;
            break;
        case MonsterType::Spikey:
            health = SPIKEY_HEALTH;
            speed = SPIKEY_SPEED;
            break;
        default:
            health = 1;
            speed = 2;
            break;
    }
    
    InitializeMonster();
}

void PrairieKing::CowboyMonster::Update(float deltaTime, const Vector2& playerPos) {
    // Update animation timer
    movementAnimationTimer += deltaTime * 1000.0f;
    if (movementAnimationTimer >= MONSTER_ANIMATION_DELAY) {
        movementAnimationTimer = 0;
    }

    // Update flash timer
    if (flashColorTimer > 0) {
        flashColorTimer -= deltaTime * 1000.0f;
    }

    // Basic AI movement towards player
    Vector2 direction = {
        playerPos.x - bounds.x,
        playerPos.y - bounds.y
    };
    
    // Normalize direction
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction.x /= length;
        direction.y /= length;
    }

    // Move monster
    Rectangle newBounds = bounds;
    newBounds.x += direction.x * speed;
    newBounds.y += direction.y * speed;
    
    // Check collisions and update position if valid
    if (!IsCollidingWithMap(newBounds)) {
        bounds = newBounds;
    }
}

void PrairieKing::CowboyMonster::Draw(const Vector2& screenOffset, Texture2D spritesheet) {
    Rectangle sourceRect;
    switch (type) {
        case MonsterType::Orc:
            sourceRect = {384 + (static_cast<int>(movementAnimationTimer / 100) % 4) * 16, 1744, 16, 16};
            break;
        case MonsterType::Ghost:
            sourceRect = {384 + (static_cast<int>(movementAnimationTimer / 100) % 4) * 16, 1760, 16, 16};
            break;
        case MonsterType::Ogre:
            sourceRect = {384 + (static_cast<int>(movementAnimationTimer / 100) % 4) * 16, 1776, 16, 16};
            break;
        case MonsterType::Spikey:
            sourceRect = {384 + (static_cast<int>(movementAnimationTimer / 100) % 4) * 16, 1792, 16, 16};
            break;
        default:
            sourceRect = {384, 1744, 16, 16};
            break;
    }
    
    Color color = (flashColorTimer > 0) ? RED : tint;
    if (invisible) color.a = 128;
    
    DrawTextureRec(spritesheet,
        sourceRect,
        {screenOffset.x + bounds.x, screenOffset.y + bounds.y},
        color);
}

bool PrairieKing::CowboyMonster::TakeDamage(int damage) {
    health -= damage;
    flashColorTimer = 100;
    return health <= 0;
}

int PrairieKing::CowboyMonster::GetLootDrop() const {
    if (GetRandomValue(0, 100) < LOOT_CHANCE * 100) {
        if (type != MonsterType::Orc && GetRandomValue(0, 100) < 10) {
            return static_cast<int>(PowerupType::Speed);
        }
        if (GetRandomValue(0, 100) < 1) {
            return static_cast<int>(PowerupType::Life);
        }
    }
    if (GetRandomValue(0, 100) < COIN_CHANCE * 100) {
        if (GetRandomValue(0, 100) < 15) {
            return static_cast<int>(PowerupType::Coin5);
        }
        return static_cast<int>(PowerupType::Coin1);
    }
    return -1;
}

void PrairieKing::CowboyMonster::InitializeMonster() {
    flashColorTimer = 0;
    movementAnimationTimer = 0;
    ticksSinceLastMovement = 0;
    movementDirection = 0;
    movedLastTurn = false;
    oppositeMotionGuy = (GetRandomValue(0, 1) == 0);
    uninterested = (GetRandomValue(0, 4) == 0);
    tint = WHITE;
}

bool PrairieKing::CowboyMonster::IsCollidingWith(const Rectangle& other) const {
    return CheckCollisionRecs(bounds, other);
}

Vector2 PrairieKing::CowboyMonster::GetPosition() const {
    return Vector2{bounds.x, bounds.y};
}

// Dracula implementation
PrairieKing::Dracula::Dracula(PrairieKing* game) 
    : CowboyMonster(MonsterType::Dracula, {8 * TILE_SIZE, 8 * TILE_SIZE})
    , m_game(game)
{
    m_phase = GLOATING_PHASE;
    m_phaseInternalTimer = 3000;
    m_phaseInternalCounter = 0;
    m_shootTimer = 0;
    health = m_fullHealth = 20;
    m_homePosition = {bounds.x, bounds.y};
}

void PrairieKing::Dracula::Update(float deltaTime, const Vector2& playerPos) {
    // Update animation and flash timers
    CowboyMonster::Update(deltaTime, playerPos);
    
    if (m_shootTimer > 0) {
        m_shootTimer -= static_cast<int>(deltaTime * 1000.0f);
    }
    
    if (m_phaseInternalTimer > 0) {
        m_phaseInternalTimer -= static_cast<int>(deltaTime * 1000.0f);
    }
    
    // State machine for Dracula's behavior phases
    switch (m_phase) {
        case GLOATING_PHASE:
            if (m_phaseInternalTimer <= 0) {
                m_phase = WALK_RANDOMLY_AND_SHOOT_PHASE;
                m_phaseInternalTimer = 5000;
            }
            break;
            
        case WALK_RANDOMLY_AND_SHOOT_PHASE:
            UpdateWalkAndShootPhase(deltaTime, playerPos);
            break;
            
        case SPREAD_SHOT_PHASE:
            UpdateSpreadShotPhase(deltaTime, playerPos);
            break;
            
        case SUMMON_DEMON_PHASE:
        case SUMMON_MUMMY_PHASE:
            UpdateSummonPhase(deltaTime, playerPos);
            break;
    }
}

void PrairieKing::Dracula::Draw(const Vector2& screenOffset, Texture2D spritesheet) {
    Rectangle sourceRect;
    if (m_phase == GLOATING_PHASE) {
        sourceRect = {448 + (static_cast<int>(movementAnimationTimer / 100) % 4) * 16, 1776, 16, 16};
    } else {
        sourceRect = {384 + (static_cast<int>(movementAnimationTimer / 100) % 4) * 16, 1776, 16, 16};
    }
    
    Color color = (flashColorTimer > 0) ? RED : WHITE;
    DrawTextureRec(spritesheet, sourceRect, 
        {screenOffset.x + bounds.x, screenOffset.y + bounds.y}, color);
}

bool PrairieKing::Dracula::TakeDamage(int damage) {
    if (m_phase == GLOATING_PHASE) {
        return false;  // Invulnerable during gloating
    }
    
    health -= damage;
    flashColorTimer = 100;
    
    // Change phase based on health percentage
    if (health < 0) {
        return true;
    } else if (health < m_fullHealth * 0.25f) {
        m_phase = SUMMON_DEMON_PHASE;
        m_phaseInternalCounter = 0;
        m_phaseInternalTimer = 2000;
    } else if (health < m_fullHealth * 0.5f) {
        m_phase = SPREAD_SHOT_PHASE;
        m_phaseInternalCounter = 0;
        m_phaseInternalTimer = 2000;
    }
    
    return false;
}

int PrairieKing::Dracula::GetLootDrop() const {
    // Drop special items on death
    if (GetRandomValue(0, 100) < 50) {
        return static_cast<int>(PowerupType::Heart);
    }
    return static_cast<int>(PowerupType::Sheriff);
}

void PrairieKing::Dracula::Update(float deltaTime, const Vector2& playerPos) {
    CowboyMonster::Update(deltaTime, playerPos);
    
    switch (m_phase) {
        case GLOATING_PHASE:
            // Just wait until timer expires
            if (m_phaseInternalTimer <= 0) {
                m_phase = WALK_RANDOMLY_AND_SHOOT_PHASE;
                m_phaseInternalTimer = 5000;
            }
            break;
            
        case WALK_RANDOMLY_AND_SHOOT_PHASE:
            UpdateWalkAndShootPhase(deltaTime, playerPos);
            break;
            
        case SPREAD_SHOT_PHASE:
            UpdateSpreadShotPhase(deltaTime, playerPos);
            break;
            
        case SUMMON_DEMON_PHASE:
        case SUMMON_MUMMY_PHASE:
            UpdateSummonPhase(deltaTime, playerPos);
            break;
    }
}

void PrairieKing::Dracula::UpdateWalkAndShootPhase(float deltaTime, const Vector2& playerPos) {
    // Move towards player
    Vector2 direction = {
        playerPos.x - bounds.x,
        playerPos.y - bounds.y
    };
    
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction.x /= length;
        direction.y /= length;
    }
    
    // Update position
    bounds.x += direction.x * speed * 1.5f; // Move faster in this phase
    bounds.y += direction.y * 1.5f;
    
    // Shoot at player
    m_shootTimer -= deltaTime * 1000.0f;
    if (m_shootTimer <= 0) {
        // Calculate direction to player for shooting
        float angle = atan2f(playerPos.y - bounds.y, playerPos.x - bounds.x);
        Vector2 bulletMotion = {
            cosf(angle) * BULLET_SPEED,
            sinf(angle) * BULLET_SPEED
        };
        
        // TODO: Add enemy bullet through PrairieKing
        m_shootTimer = 1000;
        m_phaseInternalCounter++;
    }
    
    // Change phase after enough shots
    if (m_phaseInternalCounter >= 5) {
        m_phase = SPREAD_SHOT_PHASE;
        m_phaseInternalTimer = 3000;
        m_phaseInternalCounter = 0;
    }
}

void PrairieKing::Dracula::UpdateSpreadShotPhase(float deltaTime, const Vector2& playerPos) {
    // Move back to home position
    Vector2 toHome = {
        static_cast<float>(m_homePosition.x - bounds.x),
        static_cast<float>(m_homePosition.y - bounds.y)
    };
    
    float length = sqrtf(toHome.x * toHome.x + toHome.y * toHome.y);
    if (length > 5.0f) {
        toHome.x /= length;
        toHome.y /= length;
        bounds.x += toHome.x * speed;
        bounds.y += toHome.y * speed;
    } else {
        // At home position, do spread shot
        m_shootTimer -= deltaTime * 1000.0f;
        if (m_shootTimer <= 0) {
            FireSpreadShot();
            m_shootTimer = 200;
            m_phaseInternalCounter++;
            
            if (m_phaseInternalCounter >= 16) {
                m_phase = SUMMON_DEMON_PHASE;
                m_phaseInternalTimer = 2000;
                m_phaseInternalCounter = 0;
            }
        }
    }
}

void PrairieKing::Dracula::UpdateSummonPhase(float deltaTime, const Vector2& playerPos) {
    if (m_phaseInternalTimer <= 0) {
        // Alternate between demon and mummy summons
        MonsterType summonType = (m_phase == SUMMON_DEMON_PHASE) ? 
            MonsterType::Devil : MonsterType::Mummy;
            
        SummonMinions(summonType);
        
        // Switch phases
        if (m_phase == SUMMON_DEMON_PHASE) {
            m_phase = SUMMON_MUMMY_PHASE;
        } else {
            m_phase = WALK_RANDOMLY_AND_SHOOT_PHASE;
        }
        
        m_phaseInternalTimer = 5000;
        m_phaseInternalCounter = 0;
    }
}

void PrairieKing::Dracula::FireSpreadShot() {
    const int NUM_DIRECTIONS = 8;
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        float angle = (2.0f * PI * i) / NUM_DIRECTIONS;
        Vector2 bulletMotion = {
            cosf(angle) * BULLET_SPEED,
            sinf(angle) * BULLET_SPEED
        };
        
        Vector2 spawnPos = {
            bounds.x + bounds.width/2,
            bounds.y + bounds.height/2
        };
        
        if (m_game) {
            m_game->SpawnEnemyBullet(spawnPos, bulletMotion);
        }
    }
}

void PrairieKing::Dracula::SummonMinions(MonsterType type) {
    // Summon in a cross pattern around Dracula
    Vector2 positions[] = {
        {bounds.x - TILE_SIZE * 2, bounds.y},
        {bounds.x + TILE_SIZE * 2, bounds.y},
        {bounds.x, bounds.y - TILE_SIZE * 2},
        {bounds.x, bounds.y + TILE_SIZE * 2}
    };
    
    if (m_game) {
        for (const auto& pos : positions) {
            m_game->SpawnMonsterAtPosition(type, pos);
        }
    }
}

void PrairieKing::SpawnEnemyBullet(const Vector2& position, const Vector2& motion) {
    m_enemyBullets.push_back(std::make_unique<CowboyBullet>(position, motion, 1));
}

void PrairieKing::SpawnMonsterAtPosition(MonsterType type, const Vector2& position) {
    // Don't spawn if too many monsters
    if (m_monsters.size() >= 15) return;

    // Check if spawn position is valid
    Rectangle spawnRect = {
        position.x, 
        position.y, 
        static_cast<float>(TILE_SIZE), 
        static_cast<float>(TILE_SIZE)
    };
    
    // Don't spawn if colliding with something
    for (const auto& monster : m_monsters) {
        if (CheckCollisionRecs(spawnRect, monster->GetBounds())) {
            return;
        }
    }

    m_monsters.push_back(std::make_unique<CowboyMonster>(type, position));
}

PrairieKing::Outlaw::Outlaw(PrairieKing* game)
    : CowboyMonster(MonsterType::Outlaw, {8 * TILE_SIZE, 8 * TILE_SIZE})
    , m_game(game)
{
    m_phase = TALKING_PHASE;
    m_phaseCountdown = 3000;
    m_shootTimer = 0;
    health = m_fullHealth = 30;
    m_homePosition = {bounds.x, bounds.y};
}

void PrairieKing::Outlaw::Update(float deltaTime, const Vector2& playerPos) {
    // Update base class animation and flash timers
    CowboyMonster::Update(deltaTime, playerPos);
    
    if (m_shootTimer > 0) {
        m_shootTimer -= static_cast<int>(deltaTime * 1000.0f);
    }
    
    if (m_phaseCountdown > 0) {
        m_phaseCountdown -= static_cast<int>(deltaTime * 1000.0f);
    }
    
    // State machine for Outlaw's behavior phases
    switch (m_phase) {
        case HIDING_PHASE:
            UpdateHidingPhase(deltaTime);
            break;
        case DART_OUT_AND_SHOOT_PHASE:
            UpdateDartAndShootPhase(deltaTime, playerPos);
            break;
        case RUN_AND_GUN_PHASE:
        case RUN_GUN_AND_PANT_PHASE:
            UpdateRunAndGunPhase(deltaTime, playerPos);
            break;
        case SHOOT_AT_PLAYER_PHASE:
            UpdateShootAtPlayerPhase(deltaTime, playerPos);
            break;
    }
}

// Add implementations for UpdateHidingPhase, UpdateDartAndShootPhase, etc.
// Add Draw, TakeDamage and GetLootDrop implementations

