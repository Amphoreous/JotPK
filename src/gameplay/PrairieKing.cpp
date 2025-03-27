#include "gameplay/PrairieKing.hpp"
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

void PrairieKing::CowboyPowerup::Draw(const Texture2D &cursorTexture, Vector2 topLeftScreenCoordinate)
{
    if (duration > 2000 || duration / 200 % 2 == 0)
    {
        Rectangle source = {272.0f + which * 16.0f, 1808.0f, 16.0f, 16.0f};
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
    case 0:
        motion = {0, -BULLET_SPEED}; // Use the constant
        break;
    case 1:
        motion = {BULLET_SPEED, 0};
        break;
    case 2:
        motion = {0, BULLET_SPEED};
        break;
    case 3:
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

// Initialize CowboyMonster implementation
PrairieKing::CowboyMonster::CowboyMonster(int which, int health, int speed, Vector2 position)
    : health(health), type(which), speed(speed),
      position({position.x, position.y, static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM), static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM)}),
      movementDirection(0), movedLastTurn(false), oppositeMotionGuy(false),
      invisible(false), special(false), uninterested(false), flyer(false),
      tint(WHITE), flashColor(RED), flashColorTimer(0), ticksSinceLastMovement(0),
      acceleration({0, 0}), targetPosition({0, 0}), movementAnimationTimer(0)
{
    uninterested = (rand() % 100 < 25);
}

PrairieKing::CowboyMonster::CowboyMonster(int which, Vector2 position)
    : type(which),
      position({position.x, position.y, static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM), static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM)}),
      movementDirection(0), movedLastTurn(false), oppositeMotionGuy(false),
      invisible(false), special(false), uninterested(false), flyer(false),
      tint(WHITE), flashColor(RED), flashColorTimer(0), ticksSinceLastMovement(0),
      acceleration({0, 0}), targetPosition({0, 0}), movementAnimationTimer(0)
{
    switch (type)
    {
    case ORC:
        speed = ORC_SPEED;
        health = ORC_HEALTH;
        uninterested = (rand() % 100 < 25);
        if (uninterested)
        {
            targetPosition = {static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM),
                              static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM)};
        }
        break;
    case OGRE:
        speed = OGRE_SPEED;
        health = OGRE_HEALTH;
        break;
    case MUSHROOM:
        speed = SPIKEY_SPEED;
        health = SPIKEY_HEALTH;
        break;
    case GHOST:
        speed = GHOST_SPEED;
        health = GHOST_HEALTH;
        flyer = true;
        break;
    case MUMMY:
        health = 6;
        speed = 1;
        uninterested = (rand() % 100 < 25);
        if (uninterested)
        {
            targetPosition = {static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM),
                              static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM)};
        }
        break;
    case DEVIL:
        health = 3;
        speed = 3;
        flyer = true;
        break;
    case SPIKEY:
    {
        speed = SPIKEY_SPEED;
        health = SPIKEY_HEALTH;
        int tries = 0;
        do
        {
            targetPosition = {static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM),
                              static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM)};
            tries++;
        } while (false); // We'll fix this collision check later
        break;
    }
    }
    oppositeMotionGuy = (rand() % 100 < 50);
}

void PrairieKing::CowboyMonster::Draw(const Texture2D &texture, Vector2 topLeftScreenCoordinate)
{
    if (type == SPIKEY && special)
    {
        Rectangle sourceRect;
        if (flashColorTimer > 0.0f)
        {
            sourceRect = {480.0f, 1696.0f, 16.0f, 16.0f};
        }
        else
        {
            sourceRect = {576.0f, 1712.0f, 16.0f, 16.0f};
        }

        DrawTexturePro(
            texture,
            sourceRect,
            Rectangle{topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }
    else if (!invisible)
    {
        Rectangle sourceRect;
        if (flashColorTimer > 0.0f)
        {
            sourceRect = {352.0f + type * 16.0f, 1696.0f, 16.0f, 16.0f};
        }
        else
        {
            int frame = (movementAnimationTimer < 250.0f) ? 1 : 0;
            sourceRect = {352.0f + (type * 2 + frame) * 16.0f, 1712.0f, 16.0f, 16.0f};
        }

        DrawTexturePro(
            texture,
            sourceRect,
            Rectangle{topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }
}

bool PrairieKing::CowboyMonster::TakeDamage(int damage)
{
    health -= damage;
    health = std::max(0, health);

    if (health <= 0)
    {
        return true;
    }

    // Play hit sound
    PlaySound(LoadSound("cowboy_monsterhit"));
    flashColor = RED;
    flashColorTimer = 100.0f;

    return false;
}

int PrairieKing::CowboyMonster::GetLootDrop()
{
    if (type == SPIKEY && special)
    {
        return PrairieKing::POWERUP_LOG;
    }

    if (PrairieKing::GetRandomFloat(0, 1) < 0.05)
    {
        if (type != ORC && PrairieKing::GetRandomFloat(0, 1) < 0.1)
        {
            return PrairieKing::COIN5;
        }
        if (PrairieKing::GetRandomFloat(0, 1) < 0.01)
        {
            return PrairieKing::COIN5;
        }
        return PrairieKing::COIN1;
    }

    if (PrairieKing::GetRandomFloat(0, 1) < 0.05)
    {
        if (PrairieKing::GetRandomFloat(0, 1) < 0.15)
        {
            return rand() % 2 + 6;
        }
        if (PrairieKing::GetRandomFloat(0, 1) < 0.07)
        {
            return PrairieKing::ITEM_STAR;
        }

        int loot = rand() % 8 + 2; // Range from POWERUP_SPREAD to POWERUP_TELEPORT
        if (loot == PrairieKing::POWERUP_LIFE && PrairieKing::GetRandomFloat(0, 1) < 0.4)
        {
            loot = rand() % 8 + 2;
        }
        return loot;
    }

    return -1; // No loot
}

bool PrairieKing::CowboyMonster::Move(Vector2 playerPosition, float deltaTime)
{
    // Update animation timer
    movementAnimationTimer -= deltaTime * 1000.0f;
    if (movementAnimationTimer <= 0.0f)
    {
        movementAnimationTimer = std::max(100.0f, 500.0f - speed * 50.0f);
    }

    // Handle flash color timer
    if (flashColorTimer > 0.0f)
    {
        flashColorTimer -= deltaTime * 1000.0f;
        return false;
    }

    // Skip movement if confused or in shopping mode
    if (PrairieKing::GetGameInstance() && 
        (PrairieKing::GetGameInstance()->m_monsterConfusionTimer > 0 ||
         PrairieKing::GetGameInstance()->m_shopping))
    {
        return false;
    }

    // Increment ticks since last movement
    ticksSinceLastMovement++;

    // Monster-type specific movement
    switch (type)
    {
    // Basic movement for regular monsters (orc, ogre, mushroom, mummy)
    case ORC:
    case OGRE:
    case MUSHROOM:
    case MUMMY:
    {
        // If the monster is a spikey type with special state, handle differently
        if (type == SPIKEY)
        {
            if (special || invisible)
            {
                break;
            }

            // Change target position if monster hasn't moved in a while
            if (ticksSinceLastMovement > 20)
            {
                int tries = 0;
                do
                {
                    // Pick random position on map
                    targetPosition = {
                        static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM),
                        static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM)
                    };
                    tries++;
                } while (PrairieKing::GetGameInstance()->IsCollidingWithMap(Rectangle{
                    targetPosition.x, targetPosition.y, 
                    static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM), 
                    static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM)}) && tries < 5);
            }
        }

        // Determine target position
        Vector2 target;
        if (targetPosition.x != 0 || targetPosition.y != 0)
        {
            target = targetPosition;
        }
        else
        {
            target = playerPosition;
        }

        // Handle gopher behavior
        if (PrairieKing::GetGameInstance() && PrairieKing::GetGameInstance()->m_gopherRunning)
        {
            Rectangle gopherBox = PrairieKing::GetGameInstance()->m_gopherBox;
            target = {gopherBox.x, gopherBox.y};
        }

        // Randomly change direction preference
        if (rand() % 1000 < 1)
        {
            oppositeMotionGuy = !oppositeMotionGuy;
        }

        // Determine movement direction based on target position
        if ((type == SPIKEY && !oppositeMotionGuy) || 
            fabs(target.x - position.x) > fabs(target.y - position.y))
        {
            // Horizontal movement preferred
            if (target.x + static_cast<float>(speed) < position.x && 
                (movedLastTurn || movementDirection != 3))
            {
                movementDirection = 3; // Left
            }
            else if (target.x > position.x + static_cast<float>(speed) && 
                    (movedLastTurn || movementDirection != 1))
            {
                movementDirection = 1; // Right
            }
            else if (target.y > position.y + static_cast<float>(speed) && 
                    (movedLastTurn || movementDirection != 2))
            {
                movementDirection = 2; // Down
            }
            else if (target.y + static_cast<float>(speed) < position.y && 
                    (movedLastTurn || movementDirection != 0))
            {
                movementDirection = 0; // Up
            }
        }
        else
        {
            // Vertical movement preferred
            if (target.y > position.y + static_cast<float>(speed) && 
                (movedLastTurn || movementDirection != 2))
            {
                movementDirection = 2; // Down
            }
            else if (target.y + static_cast<float>(speed) < position.y && 
                    (movedLastTurn || movementDirection != 0))
            {
                movementDirection = 0; // Up
            }
            else if (target.x + static_cast<float>(speed) < position.x && 
                    (movedLastTurn || movementDirection != 3))
            {
                movementDirection = 3; // Left
            }
            else if (target.x > position.x + static_cast<float>(speed) && 
                    (movedLastTurn || movementDirection != 1))
            {
                movementDirection = 1; // Right
            }
        }

        movedLastTurn = false;

        // Calculate attempted movement position
        Rectangle attemptedPosition = position;
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

        // Zombie mode reverses monster movement
        if (PrairieKing::GetGameInstance() && PrairieKing::GetGameInstance()->m_zombieModeTimer > 0)
        {
            // Reverse the direction
            Rectangle originalPosition = position;
            attemptedPosition.x = position.x - (attemptedPosition.x - position.x);
            attemptedPosition.y = position.y - (attemptedPosition.y - position.y);
        }

        // Check for collisions
        if (!PrairieKing::GetGameInstance()->IsCollidingWithMapForMonsters(attemptedPosition) && 
            !PrairieKing::GetGameInstance()->IsCollidingWithMonster(attemptedPosition, this))
        {
            ticksSinceLastMovement = 0;
            position = attemptedPosition;
            movedLastTurn = true;

            // Check if reached target
            if (CheckCollisionPointRec(
                Vector2{target.x + static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) / 2, 
                      target.y + static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) / 2}, 
                position))
            {
                targetPosition = {0, 0};
                
                // If monster is uninterested and reached target, pick a new random target
                if ((type == ORC || type == MUMMY) && uninterested)
                {
                    targetPosition = {
                        static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM),
                        static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM)
                    };
                    
                    // Occasionally stop being uninterested
                    if (rand() % 100 < 50)
                    {
                        uninterested = false;
                        targetPosition = {0, 0}; // Clear target to focus on player
                    }
                }
            }
        }
        break;
    }

    // Flying monsters (ghost, devil)
    case GHOST:
    case DEVIL:
    {
        // Determine target position
        Vector2 target;
        if (targetPosition.x != 0 || targetPosition.y != 0)
        {
            target = targetPosition;
        }
        else
        {
            target = playerPosition;
        }

        // Get direction vector towards target
        Vector2 targetToFly = {
            target.x + static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) / 2 - (position.x + position.width / 2),
            target.y + static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) / 2 - (position.y + position.height / 2)
        };

        // Normalize direction vector
        float length = sqrt(targetToFly.x * targetToFly.x + targetToFly.y * targetToFly.y);
        if (length > 0)
        {
            targetToFly.x = targetToFly.x / length * speed;
            targetToFly.y = targetToFly.y / length * speed;
        }

        // Adjust acceleration based on target direction
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

        // Check for collisions with new position
        Rectangle attemptedPosition = {
            position.x + ceil(acceleration.x),
            position.y + ceil(acceleration.y),
            position.width,
            position.height
        };

        if (!PrairieKing::GetGameInstance()->IsCollidingWithMonster(attemptedPosition, this))
        {
            ticksSinceLastMovement = 0;
            position.x += static_cast<int>(ceil(acceleration.x));
            position.y += static_cast<int>(ceil(acceleration.y));

            // Check if reached target
            if (CheckCollisionPointRec(
                Vector2{target.x + static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) / 2, 
                       target.y + static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) / 2}, 
                position))
            {
                targetPosition = {0, 0};
            }
        }
        break;
    }

    // Spike monster special case
    case SPIKEY:
    {
        if (!special && !invisible)
        {
            int tries = 0;
            do
            {
                targetPosition = {
                    static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM),
                    static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM)
                };
                tries++;
            } while (PrairieKing::GetGameInstance()->IsCollidingWithMap(Rectangle{
                targetPosition.x, targetPosition.y, 
                static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM), 
                static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM)}) && tries < 5);

            // Create animated spike sprite
            if (PrairieKing::GetGameInstance() && !invisible)
            {
                // Add temporary sprite animation for spike formation
                PrairieKing::TemporaryAnimatedSprite spike(
                    Rectangle{352, 1728, 16, 16},
                    60.0f, 3, 0,
                    Vector2{position.x + PrairieKing::GetGameInstance()->m_topLeftScreenCoordinate.x, position.y + PrairieKing::GetGameInstance()->m_topLeftScreenCoordinate.y},
                    0.0f, 3.0f, false,
                    position.y / 10000.0f, WHITE);
                
                // Set up callback to change state when animation ends
                spike.endFunction = [this](int extraInfo) {
                    this->SpikeyEndBehavior(extraInfo);
                };
                
                PrairieKing::GetGameInstance()->AddTemporarySprite(spike);
                invisible = true;
            }
        }
        break;
    }
    }

    return false;
}

void PrairieKing::CowboyMonster::SpikeyEndBehavior(int extraInfo)
{
    invisible = false;
    health += 5;
    special = true;
}

// Implementation of the Dracula class
PrairieKing::Dracula::Dracula()
    : CowboyMonster(-2, Vector2{static_cast<float>(8 * BASE_TILE_SIZE * PIXEL_ZOOM),
                                static_cast<float>(8 * BASE_TILE_SIZE * PIXEL_ZOOM)})
{
    homePosition = {position.x, position.y};
    position.y += static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM * 4);
    health = 350;
    fullHealth = health;
    phase = GLOATING_PHASE;
    phaseInternalTimer = 4000;
    speed = 2;
}

void PrairieKing::Dracula::Draw(const Texture2D &texture, Vector2 topLeftScreenCoordinate)
{
    if (phase != GLOATING_PHASE)
    {
        // Draw health bar
        DrawRectangle(
            static_cast<int>(topLeftScreenCoordinate.x),
            static_cast<int>(topLeftScreenCoordinate.y) + 16 * BASE_TILE_SIZE * PIXEL_ZOOM + 3,
            static_cast<int>((16 * BASE_TILE_SIZE * PIXEL_ZOOM) * (static_cast<float>(health) / static_cast<float>(fullHealth))),
            BASE_TILE_SIZE * PIXEL_ZOOM / 3,
            MAROON);
    }

    if (flashColorTimer > 0.0f)
    {
        DrawTexturePro(
            texture,
            Rectangle{464.0f, 1696.0f, 16.0f, 16.0f},
            Rectangle{topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height},
            Vector2{0, 0},
            0.0f,
            WHITE);
        return;
    }

    if (phase == GLOATING_PHASE || phase == SPREAD_SHOT_PHASE ||
        phase == SUMMON_DEMON_PHASE || phase == SUMMON_MUMMY_PHASE)
    {
        DrawTexturePro(
            texture,
            Rectangle{592.0f + (phaseInternalTimer / 100 % 3) * 16.0f, 1760.0f, 16.0f, 16.0f},
            Rectangle{topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height},
            Vector2{0, 0},
            0.0f,
            WHITE);

        if (phase == GLOATING_PHASE)
        {
            // Draw floating effect
            DrawTexturePro(
                texture,
                Rectangle{528.0f, 1776.0f, 16.0f, 16.0f},
                Rectangle{
                    topLeftScreenCoordinate.x + position.x,
                    topLeftScreenCoordinate.y + position.y + position.height + sinf(phaseInternalTimer / 1000.0f) * 3,
                    position.width, position.height},
                Vector2{0, 0},
                0.0f,
                WHITE);

            // Draw crown/hat
            DrawTexturePro(
                texture,
                Rectangle{608.0f, 1728.0f, 32.0f, 32.0f},
                Rectangle{
                    topLeftScreenCoordinate.x + position.x - static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) / 2.0f,
                    topLeftScreenCoordinate.y + position.y - static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM * 2),
                    32.0f * 3.0f, 32.0f * 3.0f},
                Vector2{0, 0},
                0.0f,
                WHITE);
        }
    }
    else
    {
        DrawTexturePro(
            texture,
            Rectangle{592.0f + (phaseInternalTimer / 100 % 2) * 16.0f, 1712.0f, 16.0f, 16.0f},
            Rectangle{topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }
}

int PrairieKing::Dracula::GetLootDrop()
{
    return POWERUP_LOG;
}

bool PrairieKing::Dracula::TakeDamage(int damage)
{
    if (phase == GLOATING_PHASE)
    {
        return false;
    }

    health -= damage;
    if (health < 0)
    {
        return true;
    }

    flashColorTimer = 100.0f;
    PlaySound(LoadSound("cowboy_monsterhit"));
    return false;
}

bool PrairieKing::Dracula::Move(Vector2 playerPosition, float deltaTime)
{
    if (flashColorTimer > 0.0f)
    {
        flashColorTimer -= deltaTime * 1000.0f;
    }

    phaseInternalTimer -= deltaTime * 1000.0f;

    // Simplified implementation for now
    return false;
}

void PrairieKing::Dracula::FireSpread(Vector2 origin, double offsetAngle)
{
    // Simplified implementation for now - will be connected to main game
    const int NUM_DIRECTIONS = 8;

    for (int i = 0; i < NUM_DIRECTIONS; i++)
    {
        float angle = static_cast<float>(i) * (2.0f * PI / NUM_DIRECTIONS);

        if (offsetAngle > 0.0)
        {
            angle += static_cast<float>(offsetAngle);
        }

        Vector2 trajectory = {
            cosf(angle) * 6.0f,
            sinf(angle) * 6.0f};

        // Bullets will be added in the final implementation
    }

    PlaySound(LoadSound("Cowboy_gunshot"));
}

void PrairieKing::Dracula::SummonEnemies(Vector2 origin, int which)
{
    int tileSize = BASE_TILE_SIZE * PIXEL_ZOOM;

    Rectangle leftSpawn = {origin.x - static_cast<float>(tileSize) - static_cast<float>(tileSize) / 2.0f, origin.y,
                           static_cast<float>(tileSize), static_cast<float>(tileSize)};
    Rectangle rightSpawn = {origin.x + static_cast<float>(tileSize) + static_cast<float>(tileSize) / 2.0f, origin.y,
                            static_cast<float>(tileSize), static_cast<float>(tileSize)};
    Rectangle bottomSpawn = {origin.x, origin.y + static_cast<float>(tileSize) + static_cast<float>(tileSize) / 2.0f,
                             static_cast<float>(tileSize), static_cast<float>(tileSize)};
    Rectangle topSpawn = {origin.x, origin.y - static_cast<float>(tileSize) - static_cast<float>(tileSize) * 3.0f / 4.0f,
                          static_cast<float>(tileSize), static_cast<float>(tileSize)};

    PlaySound(LoadSound("Cowboy_monsterDie"));
}

// Implementation of the Outlaw class
PrairieKing::Outlaw::Outlaw(Vector2 position, int health)
    : CowboyMonster(-1, position)
{
    homePosition = {position.x, position.y};
    this->health = health;
    fullHealth = health;
    phaseCountdown = 4000;
    phase = TALKING_PHASE;
}

void PrairieKing::Outlaw::Draw(const Texture2D &texture, Vector2 topLeftScreenCoordinate)
{
    // Draw health bar
    DrawRectangle(
        static_cast<int>(topLeftScreenCoordinate.x),
        static_cast<int>(topLeftScreenCoordinate.y) + 16 * BASE_TILE_SIZE * PIXEL_ZOOM + 3,
        static_cast<int>((16 * BASE_TILE_SIZE * PIXEL_ZOOM) * (static_cast<float>(health) / static_cast<float>(fullHealth))),
        BASE_TILE_SIZE * PIXEL_ZOOM / 3,
        MAROON);

    if (flashColorTimer > 0.0f)
    {
        DrawTexturePro(
            texture,
            Rectangle{496.0f, 1696.0f, 16.0f, 16.0f},
            Rectangle{topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height},
            Vector2{0, 0},
            0.0f,
            WHITE);
        return;
    }

    if (phase == TALKING_PHASE || phase == HIDING_PHASE)
    {
        DrawTexturePro(
            texture,
            Rectangle{560.0f + ((phaseCountdown / 250 % 2 == 0) ? 16.0f : 0.0f), 1776.0f, 16.0f, 16.0f},
            Rectangle{topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height},
            Vector2{0, 0},
            0.0f,
            WHITE);

        if (phase == TALKING_PHASE && phaseCountdown > 1000)
        {
            int whichWave = s_instance->m_whichWave;
            // Draw speech bubble
            DrawTexturePro(
                texture,
                Rectangle{576.0f + ((whichWave > 5) ? 32.0f : 0.0f), 1792.0f, 32.0f, 32.0f},
                Rectangle{
                    topLeftScreenCoordinate.x + position.x - static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM / 2),
                    topLeftScreenCoordinate.y + position.y - static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM * 2),
                    static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM * 3),
                    static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM * 3)},
                Vector2{0, 0},
                0.0f,
                WHITE);
        }
    }
    else if (phase == RUN_GUN_AND_PANT_PHASE && phaseInternalCounter == 2)
    {
        DrawTexturePro(
            texture,
            Rectangle{560.0f + ((phaseCountdown / 250 % 2 == 0) ? 16.0f : 0.0f), 1776.0f, 16.0f, 16.0f},
            Rectangle{topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }
    else
    {
        DrawTexturePro(
            texture,
            Rectangle{592.0f + ((phaseCountdown / 80 % 2 == 0) ? 16.0f : 0.0f), 1776.0f, 16.0f, 16.0f},
            Rectangle{topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }
}

bool PrairieKing::Outlaw::TakeDamage(int damage)
{
    if (phase == TALKING_PHASE)
    {
        return false;
    }

    health -= damage;
    if (health < 0)
    {
        return true;
    }

    flashColorTimer = 100.0f;
    PlaySound(LoadSound("cowboy_monsterhit"));
    return false;
}

int PrairieKing::Outlaw::GetLootDrop()
{
    return POWERUP_HEART;
}

bool PrairieKing::Outlaw::Move(Vector2 playerPosition, float deltaTime)
{
    if (flashColorTimer > 0.0f)
    {
        flashColorTimer -= deltaTime * 1000.0f;
    }

    phaseCountdown -= static_cast<int>(deltaTime * 1000.0f);

    // Handle different phases
    switch (phase)
    {
    case TALKING_PHASE:
        if (phaseCountdown <= 0)
        {
            phase = HIDING_PHASE;
            phaseCountdown = 1000;
        }
        break;

    case HIDING_PHASE:
        if (phaseCountdown <= 0)
        {
            phase = RUN_GUN_AND_PANT_PHASE;
            phaseCountdown = 1500;
            phaseInternalCounter = 0;
        }
        break;

    case RUN_GUN_AND_PANT_PHASE:
        if (phaseCountdown <= 0)
        {
            phaseCountdown = 2000;
            phaseInternalCounter++;

            if (phaseInternalCounter >= 3)
            {
                phase = TALKING_PHASE;
                phaseCountdown = 4000;
            }
            else
            {
                // Fire at player when phase changes
                Vector2 bulletSpawnPosition = {position.x + position.width / 2, position.y + position.height / 2};

                // Calculate direction to player
                Vector2 direction;
                direction.x = playerPosition.x - position.x;
                direction.y = playerPosition.y - position.y;

                // Normalize direction
                float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
                if (length > 0)
                {
                    direction.x /= length;
                    direction.y /= length;
                }

                // Multiple shots in different directions
                Vector2 bulletVelocity = {direction.x * 6.0f, direction.y * 6.0f};

                // Add to enemy bullets - will be done through main game instance
                PrairieKing *instance = PrairieKing::GetGameInstance();
                if (instance)
                {
                    // Add main bullet
                    instance->m_enemyBullets.push_back(CowboyBullet(bulletSpawnPosition, bulletVelocity, 1));

                    // Add slightly off-angle bullets
                    float angle = atan2f(bulletVelocity.y, bulletVelocity.x);
                    float spread = 0.2f; // 0.2 radians spread

                    Vector2 bulletVelocity1 = {
                        cosf(angle + spread) * 6.0f,
                        sinf(angle + spread) * 6.0f};

                    Vector2 bulletVelocity2 = {
                        cosf(angle - spread) * 6.0f,
                        sinf(angle - spread) * 6.0f};

                    instance->m_enemyBullets.push_back(CowboyBullet(bulletSpawnPosition, bulletVelocity1, 1));
                    instance->m_enemyBullets.push_back(CowboyBullet(bulletSpawnPosition, bulletVelocity2, 1));
                }

                PlaySound(LoadSound("Cowboy_gunshot"));
            }
        }
        break;
    }

    return false;
}

// Main PrairieKing class implementation
PrairieKing::PrairieKing(AssetManager &assets)
    : m_assets(assets),
      m_isGameOver(false),
      m_gameOver(false),
      m_quit(false),
      m_died(false),
      m_onStartMenu(false), // Changed from true to false to skip the start menu
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
      m_startTimer(0),
      m_deathTimer(0),
      m_waveTimer(WAVE_DURATION),
      m_betweenWaveTimer(BETWEEN_WAVE_DURATION),
      m_heldItem(nullptr),
      m_cactusDanceTimer(0),
      m_playerMotionAnimationTimer(0),
      m_playerFootstepSoundTimer(0),
      m_spawnQueue(4), // Initialize m_spawnQueue with 4 empty vectors
      m_overworldSong({0}) // Initialize m_overworldSong to nullptr
{
    // Store the instance pointer for static access
    s_instance = this;

    // Initialize the game
    Initialize();
}

void PrairieKing::Initialize()
{
    // Clear the button held state and frames
    m_buttonHeldState.clear();
    m_buttonHeldFrames.clear();

    for (int i = 0; i < static_cast<int>(GameKeys::MAX); i++)
    {
        m_buttonHeldFrames[static_cast<GameKeys>(i)] = 0;
    }

    // Setup border tiles - all tiles on the edge of the map
    m_borderTiles.clear();
    Rectangle mapRect = {0, 0, MAP_WIDTH, MAP_HEIGHT};
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            if (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1)
            {
                m_borderTiles.insert({static_cast<float>(x), static_cast<float>(y)});
            }
        }
    }

    // Initialize game state
    m_died = false;
    m_topLeftScreenCoordinate = {GetScreenWidth() / 2.0f - 384.0f, GetScreenHeight() / 2.0f - 384.0f};
    m_enemyBullets.clear();
    m_holdItemTimer = 0;
    m_itemToHold = -1;
    m_merchantArriving = false;
    m_merchantLeaving = false;
    m_merchantShopOpen = false;
    m_monsterConfusionTimer = 0;

    // Clear any existing monsters
    for (auto monster : m_monsters)
    {
        delete monster;
    }
    m_monsters.clear();

    m_newMapPosition = 16 * GetTileSize();
    m_scrollingMap = false;
    m_shopping = false;
    m_store = false;
    m_temporarySprites.clear();
    m_waitingForPlayerToMoveDownAMap = false;
    m_waveTimer = WAVE_DURATION;
    m_whichWave = 0;
    m_zombieModeTimer = 0;
    m_bulletDamage = 1;
    m_deathTimer = 0.0f;
    m_shootoutLevel = false;
    m_betweenWaveTimer = BETWEEN_WAVE_DURATION;
    m_gopherRunning = false;
    m_hasGopherAppeared = false;
    m_playerMovementDirections.clear();
    m_endCutscene = false;
    m_endCutscenePhase = 0;
    m_endCutsceneTimer = 0;
    m_gameOver = false;
    m_deathTimer = 0.0f;
    m_playerInvincibleTimer = 0;
    m_onStartMenu = false;
    m_startTimer = 0;
    m_powerups.clear();
    m_world = 0;

    // Initialize player position
    m_playerPosition = {384.0f, 384.0f};
    m_playerBoundingBox = {
        m_playerPosition.x + static_cast<float>(GetTileSize()) / 4.0f,
        m_playerPosition.y + static_cast<float>(GetTileSize()) / 4.0f,
        static_cast<float>(GetTileSize()) / 2.0f,
        static_cast<float>(GetTileSize()) / 2.0f};

    // Initialize spawn queues
    for (int i = 0; i < 4; i++)
    {
        m_spawnQueue[i].clear();
    }

    // Initialize boxes with proper float values
    m_noPickUpBox = {0.0f, 0.0f, static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())};
    m_merchantBox = {8.0f * static_cast<float>(GetTileSize()), 0.0f, static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())};
    m_newMapPosition = 16 * GetTileSize();

    // Initialize monster chances
    m_monsterChances = {
        {0.014f, 0.4f}, // ORC
        {0.0f, 0.0f},   // GHOST
        {0.0f, 0.0f},   // OGRE
        {0.0f, 0.0f},   // MUMMY
        {0.0f, 0.0f},   // DEVIL
        {0.0f, 0.0f},   // MUSHROOM
        {0.0f, 0.0f}    // SPIKEY
    };

    // Initialize map
    GetMap(m_whichWave, m_map);
}

bool PrairieKing::LoadGame()
{
    // In a real implementation, you'd load game data from a file
    // This is a simplified version
    return false;
}

void PrairieKing::SaveGame()
{
    // In a real implementation, you'd save game data to a file
}

void PrairieKing::Reset()
{
    Initialize();
}

void PrairieKing::ApplyNewGamePlus()
{
    m_monsterChances[0] = {0.014f + static_cast<float>(m_whichRound) * 0.005f, 0.41f + static_cast<float>(m_whichRound) * 0.05f};
    m_monsterChances[4] = {0.002f, 0.1f};
}

void PrairieKing::ApplyLevelSpecificStates()
{
    if (m_whichWave == 12)
    {
        m_shootoutLevel = true;
        m_monsters.push_back(new Dracula());

        if (m_whichRound > 0)
        {
            m_monsters.back()->health *= 2;
        }
    }
    else if (m_whichWave > 0 && m_whichWave % 4 == 0)
    {
        m_shootoutLevel = true;
        m_monsters.push_back(new Outlaw(Vector2{static_cast<float>(8 * GetTileSize()), static_cast<float>(13 * GetTileSize())}, (m_world == 0) ? 50 : 100));

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
        PlaySound(GetSound("Pickup_Coin15"));
        break;
    case COIN5:
        m_coins += 5;
        PlaySound(GetSound("Pickup_Coin15"));
        break;
    case POWERUP_LIFE:
        m_lives++;
        PlaySound(GetSound("cowboy_powerup"));
        break;
    default:
    {
        if (!m_heldItem)
        {
            m_heldItem = std::make_unique<CowboyPowerup>(c);
            PlaySound(GetSound("cowboy_powerup"));
        }
        else
        {
            // Store current held item
            CowboyPowerup oldItem = *m_heldItem;
            
            // Replace with new item
            m_heldItem = std::make_unique<CowboyPowerup>(c);
            
            // Place old item where new item was picked up
            m_noPickUpBox = {c.position.x, c.position.y, static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())};
            oldItem.position = c.position;
            m_powerups.push_back(oldItem);
            
            PlaySound(GetSound("cowboy_powerup"));
            return true;
        }
        break;
    }
    }
    return true;
}

void PrairieKing::UsePowerup(int which)
{
    // First check if powerup is already active
    if (m_activePowerups.find(which) != m_activePowerups.end())
    {
        m_activePowerups[which] = POWERUP_DURATION + 2000;
        return;
    }

    // Handle different powerup types
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
    {
        UsePowerup(POWERUP_SHOTGUN);
        UsePowerup(POWERUP_RAPIDFIRE);
        UsePowerup(POWERUP_SPEED);

        // Extend all active powerups
        for (auto &powerup : m_activePowerups)
        {
            powerup.second += 5000;
        }
        break;
    }

    case POWERUP_ZOMBIE:
        // Play zombie music
        PlaySound(GetSound("Cowboy_undead"));
        m_motionPause = 1800;
        m_zombieModeTimer = 10000;
        break;

    case POWERUP_TELEPORT:
    {
        Vector2 teleportSpot = {0, 0};
        int tries = 0;

        // Find a suitable teleport spot
        while ((fabsf(teleportSpot.x - m_playerPosition.x) < 8.0f ||
                fabsf(teleportSpot.y - m_playerPosition.y) < 8.0f ||
                IsCollidingWithMap(Rectangle{teleportSpot.x, teleportSpot.y, static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}) ||
                IsCollidingWithMonster(Rectangle{teleportSpot.x, teleportSpot.y, static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}, nullptr)) &&
               tries < 10)
        {
            teleportSpot = {
                static_cast<float>(GetRandomFloat(GetTileSize(), 16 * GetTileSize() - GetTileSize())),
                static_cast<float>(GetRandomFloat(GetTileSize(), 16 * GetTileSize() - GetTileSize()))};
            tries++;
        }

        if (tries < 10)
        {
            // Add teleport effects
            m_temporarySprites.push_back(TemporaryAnimatedSprite(
                {464, 1792, 16, 16},
                120.0f, 5, 0,
                {m_playerPosition.x + m_topLeftScreenCoordinate.x + GetTileSize() / 2,
                 m_playerPosition.y + m_topLeftScreenCoordinate.y + GetTileSize() / 2},
                0.0f, 3.0f, false,
                1.0f, WHITE));

            m_temporarySprites.push_back(TemporaryAnimatedSprite(
                {464, 1792, 16, 16},
                120.0f, 5, 0,
                {teleportSpot.x + m_topLeftScreenCoordinate.x + GetTileSize() / 2,
                 teleportSpot.y + m_topLeftScreenCoordinate.y + GetTileSize() / 2},
                0.0f, 3.0f, false,
                1.0f, WHITE));

            // Teleport animation extras around destination
            m_temporarySprites.push_back(TemporaryAnimatedSprite(
                {464, 1792, 16, 16},
                120.0f, 5, 0,
                {teleportSpot.x - GetTileSize() / 2 + m_topLeftScreenCoordinate.x + GetTileSize() / 2,
                 teleportSpot.y + m_topLeftScreenCoordinate.y + GetTileSize() / 2},
                0.0f, 3.0f, false,
                1.0f, WHITE));
            m_temporarySprites.back().delayBeforeAnimationStart = 200;

            m_temporarySprites.push_back(TemporaryAnimatedSprite(
                {464, 1792, 16, 16},
                120.0f, 5, 0,
                {teleportSpot.x + GetTileSize() / 2 + m_topLeftScreenCoordinate.x + GetTileSize() / 2,
                 teleportSpot.y + m_topLeftScreenCoordinate.y + GetTileSize() / 2},
                0.0f, 3.0f, false,
                1.0f, WHITE));
            m_temporarySprites.back().delayBeforeAnimationStart = 400;

            m_temporarySprites.push_back(TemporaryAnimatedSprite(
                {464, 1792, 16, 16},
                120.0f, 5, 0,
                {teleportSpot.x + m_topLeftScreenCoordinate.x + GetTileSize() / 2,
                 teleportSpot.y - GetTileSize() / 2 + m_topLeftScreenCoordinate.y + GetTileSize() / 2},
                0.0f, 3.0f, false,
                1.0f, WHITE));
            m_temporarySprites.back().delayBeforeAnimationStart = 600;

            m_temporarySprites.push_back(TemporaryAnimatedSprite(
                {464, 1792, 16, 16},
                120.0f, 5, 0,
                {teleportSpot.x + m_topLeftScreenCoordinate.x + GetTileSize() / 2,
                 teleportSpot.y + GetTileSize() / 2 + m_topLeftScreenCoordinate.y + GetTileSize() / 2},
                0.0f, 3.0f, false,
                1.0f, WHITE));
            m_temporarySprites.back().delayBeforeAnimationStart = 800;

            // Actually teleport the player
            m_playerPosition = teleportSpot;
            m_monsterConfusionTimer = 4000;
            m_playerInvincibleTimer = 4000;
            PlaySound(GetSound("cowboy_powerup"));
        }
        break;
    }

    case POWERUP_LIFE:
        m_lives++;
        PlaySound(GetSound("cowboy_powerup"));
        break;

    case POWERUP_NUKE:
    {
        PlaySound(GetSound("cowboy_explosion"));

        if (!m_shootoutLevel)
        {
            // Kill all monsters
            for (auto monster : m_monsters)
            {
                AddGuts({monster->position.x, monster->position.y}, monster->type);
                delete monster;
            }
            m_monsters.clear();
        }
        else
        {
            // Just damage monsters in shootout level
            for (auto monster : m_monsters)
            {
                monster->TakeDamage(2);
            }
        }

        // Add explosion effects
        for (int i = 0; i < 30; i++)
        {
            m_temporarySprites.push_back(TemporaryAnimatedSprite(
                {384, 1792, 16, 16},
                60.0f, 4, 0,
                {static_cast<float>(GetRandomFloat(GetTileSize(), 15 * GetTileSize())) + m_topLeftScreenCoordinate.x,
                 static_cast<float>(GetRandomFloat(GetTileSize(), 15 * GetTileSize())) + m_topLeftScreenCoordinate.y},
                0.0f, 3.0f, false,
                1.0f, WHITE));
        }
        break;
    }

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
        PlaySound(GetSound("Pickup_Coin15"));
        break;

    case COIN5:
        m_coins += 5;
        PlaySound(GetSound("Pickup_Coin15"));
        break;

    default:
        m_activePowerups[which] = POWERUP_DURATION;
        PlaySound(GetSound("cowboy_powerup"));
        break;
    }

    // Reduce powerup duration in new game plus
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
            Rectangle{512.0f, 1696.0f, 16.0f, 16.0f},
            80.0f, 6, 0,
            {position.x + instance->m_topLeftScreenCoordinate.x, position.y + instance->m_topLeftScreenCoordinate.y},
            0.0f, 3.0f, GetRandomFloat(0, 1) < 0.5f,
            0.001f, WHITE);
        instance->AddTemporarySprite(blood);

        // Lingering guts
        TemporaryAnimatedSprite guts(
            Rectangle{592.0f, 1696.0f, 16.0f, 16.0f},
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
            Rectangle{464.0f, 1792.0f, 16.0f, 16.0f},
            80.0f, 5, 0,
            {position.x + instance->m_topLeftScreenCoordinate.x, position.y + instance->m_topLeftScreenCoordinate.y},
            0.0f, 3.0f, GetRandomFloat(0, 1) < 0.5f,
            0.001f, WHITE);
        instance->AddTemporarySprite(mummyDeath);
        break;
    }

    case GHOST:
    case DEVIL:
    {
        // Ghost/Devil specific death animation
        TemporaryAnimatedSprite ghostDeath(
            Rectangle{544.0f, 1728.0f, 16.0f, 16.0f},
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
        {384, 1792, 16, 16},
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
    for (int i = m_bullets.size() - 1; i >= 0; i--)
    {
        // Move the bullet
        m_bullets[i].position.x += m_bullets[i].motion.x;
        m_bullets[i].position.y += m_bullets[i].motion.y;

        // Check if bullet is off screen
        if (m_bullets[i].position.x <= 0 || m_bullets[i].position.y <= 0 ||
            m_bullets[i].position.x >= 768 || m_bullets[i].position.y >= 768)
        {
            m_bullets.erase(m_bullets.begin() + i);
            continue;
        }

        // Check if bullet hit fence
        if (m_map[static_cast<int>((m_bullets[i].position.x + 6) / (16 * 3))][static_cast<int>((m_bullets[i].position.y + 6) / (16 * 3))] == MAP_FENCE)
        {
            m_bullets.erase(m_bullets.begin() + i);
            continue;
        }

        // Check if bullet hit a monster
        bool hitMonster = false;

        for (int j = m_monsters.size() - 1; j >= 0; j--)
        {
            if (CheckCollisionPointRec(
                    {m_bullets[i].position.x, m_bullets[i].position.y},
                    m_monsters[j]->position))
            {
                // Hit a monster
                hitMonster = true;

                // Deal damage to monster
                if (m_monsters[j]->TakeDamage(m_bullets[i].damage))
                {
                    // Monster is killed
                    int lootDrop = m_monsters[j]->GetLootDrop();

                    if (lootDrop != -1)
                    {
                        // Add powerup
                        m_powerups.push_back(CowboyPowerup(
                            lootDrop,
                            {m_monsters[j]->position.x, m_monsters[j]->position.y},
                            LOOT_DURATION));
                    }

                    // Add blood/guts effect
                    AddGuts({m_monsters[j]->position.x, m_monsters[j]->position.y}, m_monsters[j]->type);

                    // Check if it was an outlaw or dracula
                    if (m_monsters[j]->type == -1) // Outlaw
                    {
                        KillOutlaw();
                    }
                    else
                    {
                        // Play death sound
                        PlaySound(PrairieKing::GetSoundStatic("Cowboy_monsterDie"));

                        // Delete the monster
                        delete m_monsters[j];
                        m_monsters.erase(m_monsters.begin() + j);
                    }
                }

                break;
            }
        }

        if (hitMonster)
        {
            m_bullets.erase(m_bullets.begin() + i);
        }
    }

    // Update enemy bullets
    for (int i = m_enemyBullets.size() - 1; i >= 0; i--)
    {
        // Move the bullet
        m_enemyBullets[i].position.x += m_enemyBullets[i].motion.x;
        m_enemyBullets[i].position.y += m_enemyBullets[i].motion.y;

        // Check if bullet is off screen
        if (m_enemyBullets[i].position.x <= 0 || m_enemyBullets[i].position.y <= 0 ||
            m_enemyBullets[i].position.x >= 762 || m_enemyBullets[i].position.y >= 762)
        {
            m_enemyBullets.erase(m_enemyBullets.begin() + i);
            continue;
        }

        // Check if bullet hit fence
        if (m_map[static_cast<int>((m_enemyBullets[i].position.x + 6) / (16 * 3))][static_cast<int>((m_enemyBullets[i].position.y + 6) / (16 * 3))] == MAP_FENCE)
        {
            m_enemyBullets.erase(m_enemyBullets.begin() + i);
            continue;
        }

        // Check if bullet hit player (and player is not invincible)
        if (m_playerInvincibleTimer <= 0 && m_deathTimer <= 0.0f &&
            CheckCollisionRecs(
                m_playerBoundingBox,
                {m_enemyBullets[i].position.x, m_enemyBullets[i].position.y, 15, 15}))
        {
            PlayerDie();
            break;
        }
    }
}

void PrairieKing::PlayerDie()
{
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

    // Add death animation
    m_temporarySprites.push_back(TemporaryAnimatedSprite(
        {464, 1808, 16, 16},
        120.0f, 5, 0,
        {m_playerPosition.x + m_topLeftScreenCoordinate.x, m_playerPosition.y + m_topLeftScreenCoordinate.y},
        0.0f, 3.0f, false,
        1.0f, WHITE));

    // Make enemies wait longer between waves
    m_waveTimer = std::min(WAVE_DURATION, m_waveTimer + 10000);
    m_betweenWaveTimer = 4000;

    // Lose a life
    m_lives--;
    m_playerInvincibleTimer = 5000;

    if (m_shootoutLevel)
    {
        m_playerPosition = {static_cast<float>(8 * GetTileSize()), static_cast<float>(3 * GetTileSize())};
        PlaySound(PrairieKing::GetSoundStatic("Cowboy_monsterDie"));
    }
    else
    {
        m_playerPosition = {static_cast<float>(8 * GetTileSize() - GetTileSize()), static_cast<float>(8 * GetTileSize())};
        m_playerBoundingBox.x = static_cast<int>(m_playerPosition.x) + GetTileSize() / 4;
        m_playerBoundingBox.y = static_cast<int>(m_playerPosition.y) + GetTileSize() / 4;
        PlaySound(GetSound("cowboy_dead"));
    }

    if (m_lives < 0)
    {
        // Game over
        m_temporarySprites.push_back(TemporaryAnimatedSprite(
            {464, 1808, 16, 16},
            550.0f, 5, 0,
            {m_playerPosition.x + m_topLeftScreenCoordinate.x, m_playerPosition.y + m_topLeftScreenCoordinate.y},
            0.0f, 3.0f, false,
            1.0f, WHITE));
        m_temporarySprites.back().alpha = 0.001f;
        m_temporarySprites.back().endFunction = [this](int extra)
        { AfterPlayerDeathFunction(extra); };

        m_deathTimer *= 3.0f;
    }
    else if (!m_shootoutLevel)
    {
        SaveGame();
    }
}

void PrairieKing::AfterPlayerDeathFunction(int extra)
{
    if (m_lives < 0)
    {
        m_gameOver = true;

        // Clear monsters and powerups
        for (auto monster : m_monsters)
        {
            delete monster;
        }
        m_monsters.clear();
        m_powerups.clear();
        m_died = false;

        PlaySound(PrairieKing::GetSoundStatic("Cowboy_monsterDie"));
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
    if (IsKeyDown(GameKeys::MoveUp))
    {
        if (IsKeyPressed(GameKeys::MoveUp) && m_gameOver)
        {
            m_gameOverOption = std::max(0, m_gameOverOption - 1);
            PlaySound(PrairieKing::GetSoundStatic("Cowboy_gunshot"));
        }
        AddPlayerMovementDirection(0);
    }
    else if (std::find(m_playerMovementDirections.begin(), m_playerMovementDirections.end(), 0) != m_playerMovementDirections.end())
    {
        m_playerMovementDirections.erase(std::remove(m_playerMovementDirections.begin(), m_playerMovementDirections.end(), 0), m_playerMovementDirections.end());
    }

    if (IsKeyDown(GameKeys::MoveDown))
    {
        if (IsKeyPressed(GameKeys::MoveDown) && m_gameOver)
        {
            m_gameOverOption = std::min(1, m_gameOverOption + 1);
            PlaySound(PrairieKing::GetSoundStatic("Cowboy_gunshot"));
        }
        AddPlayerMovementDirection(2);
    }
    else if (std::find(m_playerMovementDirections.begin(), m_playerMovementDirections.end(), 2) != m_playerMovementDirections.end())
    {
        m_playerMovementDirections.erase(std::remove(m_playerMovementDirections.begin(), m_playerMovementDirections.end(), 2), m_playerMovementDirections.end());
    }

    if (IsKeyDown(GameKeys::MoveLeft))
    {
        AddPlayerMovementDirection(3);
    }
    else if (std::find(m_playerMovementDirections.begin(), m_playerMovementDirections.end(), 3) != m_playerMovementDirections.end())
    {
        m_playerMovementDirections.erase(std::remove(m_playerMovementDirections.begin(), m_playerMovementDirections.end(), 3), m_playerMovementDirections.end());
    }

    if (IsKeyDown(GameKeys::MoveRight))
    {
        AddPlayerMovementDirection(1);
    }
    else if (std::find(m_playerMovementDirections.begin(), m_playerMovementDirections.end(), 1) != m_playerMovementDirections.end())
    {
        m_playerMovementDirections.erase(std::remove(m_playerMovementDirections.begin(), m_playerMovementDirections.end(), 1), m_playerMovementDirections.end());
    }

    if (IsKeyDown(GameKeys::ShootUp))
    {
        AddPlayerShootingDirection(0);
    }
    else if (std::find(m_playerShootingDirections.begin(), m_playerShootingDirections.end(), 0) != m_playerShootingDirections.end())
    {
        m_playerShootingDirections.erase(std::remove(m_playerShootingDirections.begin(), m_playerShootingDirections.end(), 0), m_playerShootingDirections.end());
    }

    if (IsKeyDown(GameKeys::ShootDown))
    {
        AddPlayerShootingDirection(2);
    }
    else if (std::find(m_playerShootingDirections.begin(), m_playerShootingDirections.end(), 2) != m_playerShootingDirections.end())
    {
        m_playerShootingDirections.erase(std::remove(m_playerShootingDirections.begin(), m_playerShootingDirections.end(), 2), m_playerShootingDirections.end());
    }

    if (IsKeyDown(GameKeys::ShootLeft))
    {
        AddPlayerShootingDirection(3);
    }
    else if (std::find(m_playerShootingDirections.begin(), m_playerShootingDirections.end(), 3) != m_playerShootingDirections.end())
    {
        m_playerShootingDirections.erase(std::remove(m_playerShootingDirections.begin(), m_playerShootingDirections.end(), 3), m_playerShootingDirections.end());
    }

    if (IsKeyDown(GameKeys::ShootRight))
    {
        AddPlayerShootingDirection(1);
    }
    else if (std::find(m_playerShootingDirections.begin(), m_playerShootingDirections.end(), 1) != m_playerShootingDirections.end())
    {
        m_playerShootingDirections.erase(std::remove(m_playerShootingDirections.begin(), m_playerShootingDirections.end(), 1), m_playerShootingDirections.end());
    }

    if (IsKeyPressed(GameKeys::SelectOption) && m_gameOver)
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
            PlaySound(GetSound("Pickup_Coin15"));
        }
    }

    if (IsKeyPressed(GameKeys::UsePowerup) && !m_gameOver && m_heldItem != nullptr && m_deathTimer <= 0.0f && m_zombieModeTimer <= 0)
    {
        UsePowerup(m_heldItem->which);
        m_heldItem.reset(); // Replace delete m_heldItem with reset() for unique_ptr
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
    if (directions.empty()) {
        return;
    }

    int playerShootingDirection = directions[0];
    
    // Calculate bullet starting position
    Vector2 bulletSpawn = {
        spawn.x + static_cast<float>(GetTileSize()) / 2.0f,
        spawn.y + static_cast<float>(GetTileSize()) / 2.0f
    };

    // Determine bullet count based on powerups
    int bulletCount = 1;
    if (m_activePowerups.find(POWERUP_SHOTGUN) != m_activePowerups.end()) {
        bulletCount = 3;
    }

    if (directions.size() == 1) {
        // Single direction
        for (int i = 0; i < bulletCount; i++) {
            Vector2 bulletMotion = {0, 0};
            
            switch (playerShootingDirection) {
            case 0: // Up
                bulletMotion = {i == 0 ? 0.0f : (i == 1 ? -2.0f : 2.0f), -BULLET_SPEED};
                break;
            case 1: // Right
                bulletMotion = {BULLET_SPEED, i == 0 ? 0.0f : (i == 1 ? -2.0f : 2.0f)};
                break;
            case 2: // Down
                bulletMotion = {i == 0 ? 0.0f : (i == 1 ? -2.0f : 2.0f), BULLET_SPEED};
                break;
            case 3: // Left
                bulletMotion = {-BULLET_SPEED, i == 0 ? 0.0f : (i == 1 ? -2.0f : 2.0f)};
                break;
            }

            // Only apply spread pattern if we have shotgun or multiple bullets
            if (bulletCount == 1 || i == 0) {
                m_bullets.push_back(CowboyBullet(bulletSpawn, bulletMotion, m_bulletDamage));
            }
            else if (bulletCount > 1) {
                m_bullets.push_back(CowboyBullet(bulletSpawn, bulletMotion, m_bulletDamage));
            }
        }
    }
    else {
        // Handle spread powerup for diagonal shooting
        bool hasSpread = m_activePowerups.find(POWERUP_SPREAD) != m_activePowerups.end();
        
        for (int dir : directions) {
            Vector2 bulletMotion = {0, 0};
            
            switch (dir) {
            case 0: // Up
                bulletMotion = {0, -BULLET_SPEED};
                break;
            case 1: // Right
                bulletMotion = {BULLET_SPEED, 0};
                break;
            case 2: // Down
                bulletMotion = {0, BULLET_SPEED};
                break;
            case 3: // Left
                bulletMotion = {-BULLET_SPEED, 0};
                break;
            }
            
            m_bullets.push_back(CowboyBullet(bulletSpawn, bulletMotion, m_bulletDamage));
            
            // Add diagonal bullets for spread powerup
            if (hasSpread && bulletCount > 1) {
                // Add diagonal variants based on direction
                if (dir == 0 || dir == 2) { // Up or Down
                    m_bullets.push_back(CowboyBullet(bulletSpawn, 
                        {bulletMotion.x - 2.0f, bulletMotion.y}, m_bulletDamage));
                    m_bullets.push_back(CowboyBullet(bulletSpawn, 
                        {bulletMotion.x + 2.0f, bulletMotion.y}, m_bulletDamage));
                } else { // Left or Right
                    m_bullets.push_back(CowboyBullet(bulletSpawn, 
                        {bulletMotion.x, bulletMotion.y - 2.0f}, m_bulletDamage));
                    m_bullets.push_back(CowboyBullet(bulletSpawn, 
                        {bulletMotion.x, bulletMotion.y + 2.0f}, m_bulletDamage));
                }
            }
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
    if (!m_gopherTrain && std::find(m_playerMovementDirections.begin(), m_playerMovementDirections.end(), direction) == m_playerMovementDirections.end())
    {
        if (m_playerMovementDirections.size() == 1 && direction == (m_playerMovementDirections[0] + 2) % 4)
        {
            // If trying to move in the opposite direction, replace the current direction
            m_playerMovementDirections[0] = direction;
        }
        else if (m_playerMovementDirections.size() < 2)
        {
            m_playerMovementDirections.push_back(direction);
        }
    }
}

void PrairieKing::AddPlayerShootingDirection(int direction)
{
    if (std::find(m_playerShootingDirections.begin(), m_playerShootingDirections.end(), direction) == m_playerShootingDirections.end())
    {
        if (m_playerShootingDirections.size() == 1 && direction == (m_playerShootingDirections[0] + 2) % 4)
        {
            // If trying to shoot in the opposite direction, replace the current direction
            m_playerShootingDirections[0] = direction;
        }
        else if (m_playerShootingDirections.size() < 2)
        {
            m_playerShootingDirections.push_back(direction);
        }
    }
}

void PrairieKing::StartShoppingLevel()
{
    // Stop any music

    // Choose store items based on round and world
    m_storeItems.clear();

    if (m_world == WOODS)
    {
        m_storeItems[Rectangle{static_cast<float>(3 * GetTileSize()), static_cast<float>(10 * GetTileSize()), static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}] = ITEM_FIRESPEED2;
        m_storeItems[Rectangle{static_cast<float>(5 * GetTileSize()), static_cast<float>(10 * GetTileSize()), static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}] = ITEM_RUNSPEED2;
        m_storeItems[Rectangle{static_cast<float>(7 * GetTileSize()), static_cast<float>(10 * GetTileSize()), static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}] = ITEM_AMMO2;
        m_storeItems[Rectangle{static_cast<float>(9 * GetTileSize()), static_cast<float>(10 * GetTileSize()), static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}] = ITEM_SPREADPISTOL;
        m_storeItems[Rectangle{static_cast<float>(11 * GetTileSize()), static_cast<float>(10 * GetTileSize()), static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}] = ITEM_LIFE;
    }
    else
    {
        m_storeItems[Rectangle{static_cast<float>(3 * GetTileSize()), static_cast<float>(10 * GetTileSize()), static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}] = ITEM_FIRESPEED1;
        m_storeItems[Rectangle{static_cast<float>(5 * GetTileSize()), static_cast<float>(10 * GetTileSize()), static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}] = ITEM_RUNSPEED1;
        m_storeItems[Rectangle{static_cast<float>(7 * GetTileSize()), static_cast<float>(10 * GetTileSize()), static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}] = ITEM_AMMO1;
        m_storeItems[Rectangle{static_cast<float>(9 * GetTileSize()), static_cast<float>(10 * GetTileSize()), static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}] = ITEM_LIFE;
        m_storeItems[Rectangle{static_cast<float>(11 * GetTileSize()), static_cast<float>(10 * GetTileSize()), static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}] = ITEM_STAR;
    }

    if (m_whichRound > 0)
    {
        // In new gameplus, adjust store items
        m_storeItems[Rectangle{static_cast<float>(11 * GetTileSize()), static_cast<float>(10 * GetTileSize()), static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())}] = (m_world == WOODS) ? ITEM_SKULL : ITEM_LOG;
    }

    m_shopping = true;
    m_merchantArriving = true;
    m_merchantLeaving = false;
    m_merchantShopOpen = false;
    m_shoppingTimer = 0;
}

int PrairieKing::GetPriceForItem(int whichItem)
{
    switch (whichItem)
    {
    case ITEM_AMMO1:
        return 7;
    case ITEM_AMMO2:
        return 15;
    case ITEM_AMMO3:
        return 25;
    case ITEM_FIRESPEED1:
        return 10;
    case ITEM_FIRESPEED2:
        return 25;
    case ITEM_FIRESPEED3:
        return 50;
    case ITEM_LIFE:
        return 30;
    case ITEM_RUNSPEED1:
        return 12;
    case ITEM_RUNSPEED2:
        return 30;
    case ITEM_SPREADPISTOL:
        return 45;
    case ITEM_STAR:
        return 20;
    case ITEM_SKULL:
    case ITEM_LOG:
        return 666;
    default:
        return 999;
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
                newMap[x][y] = MAP_CACTUS;
            }
            else if (x == 0 || x == 15 || y == 0 || y == 15)
            {
                // Random barrier type for edges
                newMap[x][y] = (GetRandomFloat(0, 1) < 0.15f) ? MAP_BARRIER2 : MAP_BARRIER1;
            }
            else if (x == 1 || x == 14 || y == 1 || y == 14)
            {
                newMap[x][y] = MAP_ROCKY1;
            }
            else
            {
                // Random terrain for interior
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
                if (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1)
                {
                    newMap[x][y] = MAP_BARRIER1;
                }
                else if (y == MAP_HEIGHT - 1)
                {
                    newMap[x][y] = MAP_GRASSY;
                }
                else
                {
                    newMap[x][y] = MAP_DESERT;
                }
            }
        }

        // Add some details
        newMap[3][1] = MAP_CACTUS;
        newMap[8][2] = MAP_CACTUS;
        newMap[13][1] = MAP_CACTUS;
        newMap[5][0] = MAP_BARRIER1;
        newMap[10][2] = MAP_ROCKY1;
        newMap[15][2] = MAP_BARRIER2;
        newMap[14][12] = MAP_CACTUS;

        // Add fence
        for (int i = 10; i <= 14; i++)
        {
            newMap[i][6] = MAP_FENCE;
        }
        for (int i = 7; i <= 13; i++)
        {
            newMap[14][i] = MAP_FENCE;
        }

        // Add other details
        newMap[3][3] = MAP_BRIDGE;
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
        // Add central barriers
        newMap[8][4] = MAP_FENCE;
        newMap[12][8] = MAP_FENCE;
        newMap[8][12] = MAP_FENCE;
        newMap[4][8] = MAP_FENCE;

        // Add corner cacti
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
        break;

    case 3:
        // Fence perimeters in the middle
        for (int i = 5; i <= 11; i++)
        {
            if (i != 8)
            {
                newMap[i][5] = MAP_FENCE;
                newMap[i][11] = MAP_FENCE;
            }
        }

        for (int i = 6; i <= 10; i++)
        {
            if (i != 8)
            {
                newMap[5][i] = MAP_FENCE;
                newMap[11][i] = MAP_FENCE;
            }
        }
        break;

    case 4:
    case 8:
        // Create a shootout alley
        for (int i = 0; i < MAP_WIDTH; i++)
        {
            newMap[i][6] = MAP_FENCE;
            newMap[i][10] = MAP_FENCE;
        }

        // Add gaps
        newMap[8][6] = MAP_DESERT;
        newMap[8][10] = MAP_DESERT;

        // Add small openings in fence for the player
        newMap[8][4] = MAP_FENCE;
        newMap[8][12] = MAP_FENCE;
        newMap[9][12] = MAP_FENCE;
        newMap[7][12] = MAP_FENCE;

        // Add cacti
        newMap[5][6] = MAP_CACTUS;
        newMap[10][6] = MAP_CACTUS;
        break;

    case 5:
        // Add corner cacti
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
        // Add rocks and cacti
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
        // Add trenches
        for (int i = 1; i < MAP_WIDTH - 1; i++)
        {
            newMap[i][7] = MAP_TRENCH1;
            newMap[i][8] = MAP_TRENCH2;
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
        // Add trench river through the middle
        for (int i = 1; i < MAP_WIDTH - 1; i++)
        {
            newMap[i][7] = MAP_TRENCH1;
            newMap[i][8] = MAP_TRENCH2;
        }

        // Add bridges
        newMap[8][1] = MAP_BRIDGE;
        newMap[7][1] = MAP_BRIDGE;
        newMap[9][1] = MAP_BRIDGE;
        newMap[8][14] = MAP_BRIDGE;
        newMap[7][14] = MAP_BRIDGE;
        newMap[9][14] = MAP_BRIDGE;

        // Add some cacti
        newMap[6][8] = MAP_CACTUS;
        newMap[10][8] = MAP_CACTUS;
        newMap[8][6] = MAP_CACTUS;
        newMap[8][9] = MAP_CACTUS;
        break;

    case 11:
        // Alternating fence pattern
        for (int i = 0; i < MAP_WIDTH; i++)
        {
            if (i % 2 == 0)
            {
                newMap[i][5] = MAP_FENCE;
                newMap[i][11] = MAP_FENCE;
            }
            else
            {
                newMap[i][3] = MAP_FENCE;
                newMap[i][13] = MAP_FENCE;
            }
        }
        break;

    case 12:
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
        for (int i = 0; i < MAP_WIDTH; i++)
        {
            newMap[i][0] = (i % 2 == 0) ? MAP_TRENCH1 : MAP_TRENCH2;
            newMap[i][MAP_HEIGHT - 1] = (i % 2 == 0) ? MAP_TRENCH1 : MAP_TRENCH2;
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

bool PrairieKing::IsKeyPressed(GameKeys key)
{
    // Only return true if the key was just pressed this frame
    if (IsKeyDown(key) && m_buttonHeldFrames[key] == 1)
    {
        return true;
    }
    return false;
}

bool PrairieKing::IsKeyDown(GameKeys key)
{
    return m_buttonHeldState.find(key) != m_buttonHeldState.end();
}

bool PrairieKing::IsKeyReleased(GameKeys key)
{
    return !IsKeyDown(key) && m_buttonHeldFrames[key] == 0;
}

void PrairieKing::Update(float deltaTime)
{
    // Update input state
    for (int i = 0; i < static_cast<int>(GameKeys::MAX); i++)
    {
        GameKeys key = static_cast<GameKeys>(i);
        if (IsKeyDown(key))
        {
            m_buttonHeldFrames[key]++;
        }
        else
        {
            m_buttonHeldFrames[key] = 0;
        }
    }

    // Update cactus dance timer
    m_cactusDanceTimer += deltaTime * 1000.0f;
    if (m_cactusDanceTimer >= CACTUS_DANCE_DELAY * 2)
    {
        m_cactusDanceTimer = 0.0f;
    }

    // Process player inputs first
    ProcessInputs();

    if (m_quit)
        return;

    // Update timers
    if (m_motionPause > 0)
    {
        m_motionPause -= static_cast<int>(deltaTime * 1000.0f);
        if (m_motionPause <= 0 && m_behaviorAfterPause)
        {
            m_behaviorAfterPause(0);
            m_behaviorAfterPause = nullptr;
        }
        return;
    }

    if (m_playerInvincibleTimer > 0)
    {
        m_playerInvincibleTimer -= static_cast<int>(deltaTime * 1000.0f);
    }

    if (m_zombieModeTimer > 0)
    {
        m_zombieModeTimer -= static_cast<int>(deltaTime * 1000.0f);
    }

    if (m_holdItemTimer > 0)
    {
        m_holdItemTimer -= static_cast<int>(deltaTime * 1000.0f);
        return;
    }

    if (m_screenFlash > 0)
    {
        m_screenFlash -= static_cast<int>(deltaTime * 1000.0f);
    }

    // Update powerups durations
    for (auto it = m_activePowerups.begin(); it != m_activePowerups.end();)
    {
        it->second -= static_cast<int>(deltaTime * 1000.0f);
        if (it->second <= 0)
        {
            it = m_activePowerups.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Update powerups
    for (int i = m_powerups.size() - 1; i >= 0; i--)
    {
        m_powerups[i].duration -= static_cast<int>(deltaTime * 1000.0f);
        if (m_powerups[i].duration <= 0)
        {
            m_powerups.erase(m_powerups.begin() + i);
        }
    }

    // Update game state
    if (!m_gameOver && !m_onStartMenu)
    {
        // Update bullets
        UpdateBullets(deltaTime);

        // Update monsters
        for (auto monster : m_monsters)
        {
            monster->Move(m_playerPosition, deltaTime);
        }

        // Update temporary sprites
        for (int i = m_temporarySprites.size() - 1; i >= 0; i--)
        {
            if (m_temporarySprites[i].Update(deltaTime))
            {
                m_temporarySprites.erase(m_temporarySprites.begin() + i);
            }
        }

        // Handle scrolling map
        if (m_scrollingMap)
        {
            // Decrease new map position at consistent rate (move map up)
            m_newMapPosition -= GetTileSize() / 8;
            
            // Move player position DOWN slightly but not too much (like in original game)
            m_playerPosition.y -= GetTileSize() / 8;
            m_playerPosition.y += 3.0f;
            
            // Update player bounding box with new position
            m_playerBoundingBox.x = m_playerPosition.x + GetTileSize() / 4;
            m_playerBoundingBox.y = m_playerPosition.y + GetTileSize() / 4;
            m_playerBoundingBox.width = GetTileSize() / 2;
            m_playerBoundingBox.height = GetTileSize() / 2;
            
            // Keep player walking down during scrolling
            if (m_playerMovementDirections.empty()) {
                m_playerMovementDirections.push_back(2); // Down direction
            }
            
            // Update animation timer
            m_playerMotionAnimationTimer += deltaTime * 1000.0f;
            m_playerMotionAnimationTimer = fmodf(m_playerMotionAnimationTimer, 400.0f);
            
            // When scrolling completes
            if (m_newMapPosition <= 0)
            {
                m_scrollingMap = false;
                // Copy next map to current map
                memcpy(m_map, m_nextMap, sizeof(m_map));
                m_newMapPosition = 16 * GetTileSize();
                m_shopping = false;
                m_betweenWaveTimer = BETWEEN_WAVE_DURATION;
                m_waitingForPlayerToMoveDownAMap = false;
                m_playerMovementDirections.clear();
                
                // Reset animation timers
                m_playerMotionAnimationTimer = 0.0f;
                m_playerFootstepSoundTimer = 200.0f;
                
                // Clear any projectiles that went off screen
                m_bullets.erase(
                    std::remove_if(m_bullets.begin(), m_bullets.end(),
                        [this](const CowboyBullet& bullet) {
                            return bullet.position.y > 16.0f * GetTileSize();
                        }), 
                    m_bullets.end()
                );
                
                // Apply level-specific states after scrolling completes
                ApplyLevelSpecificStates();
            }
            else 
            {
                // During scrolling, adjust collision checks for monsters and powerups
                for (auto& monster : m_monsters)
                {
                    monster->position.y -= GetTileSize() / 8;
                }
                
                for (auto& powerup : m_powerups)
                {
                    powerup.position.y -= GetTileSize() / 8;
                }
                
                // Update bullet positions during scroll
                for (auto& bullet : m_bullets)
                {
                    if (bullet.position.y > 16.0f * GetTileSize())
                    {
                        bullet.position.y -= GetTileSize() / 8;
                    }
                }
            }
        }
        else
        {
            // Update wave timer only if not scrolling map
            if (m_waveTimer > 0)
            {
                m_waveTimer -= static_cast<int>(deltaTime * 1000.0f);
                if (m_waveTimer <= 0)
                {
                    StartNewWave();
                }
            }

            // Update between wave timer
            if (m_betweenWaveTimer > 0 && m_monsters.empty() && IsSpawnQueueEmpty() && !m_shopping && !m_waitingForPlayerToMoveDownAMap)
            {
                m_betweenWaveTimer -= static_cast<int>(deltaTime * 1000.0f);
                
                // If the between wave timer has completed and we're at wave 0 (start of game)
                // then start the first wave
                if (m_betweenWaveTimer <= 0 && m_whichWave == 0)
                {
                    m_whichWave = 0; // Keep it at 0 for the first playable level
                    m_waveTimer = WAVE_DURATION; // Reset the wave timer for level 0
                    
                    // Don't immediately go to the next level
                    m_waitingForPlayerToMoveDownAMap = false;
                }
            }
            
            // Play overworld music if needed, but only if not between waves at the start
            if (m_waveTimer > 0 && 
                m_betweenWaveTimer <= 0 && 
                m_zombieModeTimer <= 0 && 
                !m_shootoutLevel && 
                m_overworldSong.frameCount == 0 || !IsSoundPlaying(m_overworldSong)) 
            {
                m_overworldSong = GetSound("Cowboy_OVERWORLD");
                PlaySound(m_overworldSong);
            }
        }
    }

    // Update player movement and shooting
    UpdatePlayer(deltaTime);
}

void PrairieKing::UpdatePlayer(float deltaTime)
{
    if (m_deathTimer > 0.0f)
    {
        m_deathTimer -= deltaTime * 1000.0f;
        return;
    }

    // Collect powerups if player is over them
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

    // Clear no pickup box if player is not colliding with it
    if (!CheckCollisionRecs(m_playerBoundingBox, m_noPickUpBox))
    {
        m_noPickUpBox = {0.0f, 0.0f, static_cast<float>(GetTileSize()), static_cast<float>(GetTileSize())};
    }

    // Handle movement
    if (!m_playerMovementDirections.empty() && !m_scrollingMap && m_deathTimer <= 0.0f)
    {
        // Calculate effective directions for movement
        int effectiveDirections = m_playerMovementDirections.size();
        if (effectiveDirections >= 2 && m_playerMovementDirections.back() == (m_playerMovementDirections[m_playerMovementDirections.size() - 2] + 2) % 4)
        {
            effectiveDirections = 1;
        }

        // Calculate movement speed
        float speed = GetMovementSpeed(3.0f, effectiveDirections);

        // Apply speed from powerups
        if (m_activePowerups.find(POWERUP_SPEED) != m_activePowerups.end())
        {
            speed *= 1.5f;
        }

        if (m_zombieModeTimer > 0)
        {
            speed *= 1.5f;
        }

        // Apply run speed level boost
        for (int i = 0; i < m_runSpeedLevel; i++)
        {
            speed *= 1.25f;
        }

        // Move player in each direction (handling simultaneous keypresses)
        for (int i = std::max(0, static_cast<int>(m_playerMovementDirections.size()) - 2);
             i < static_cast<int>(m_playerMovementDirections.size()); i++)
        {
            // Skip if this would cause cancellation with opposite direction
            if (i != 0 || m_playerMovementDirections.size() < 2 ||
                m_playerMovementDirections.back() != (m_playerMovementDirections[m_playerMovementDirections.size() - 2] + 2) % 4)
            {
                Vector2 newPosition = m_playerPosition;

                // Apply movement based on direction
                switch (m_playerMovementDirections[i])
                {
                case 0: // Up
                    newPosition.y -= speed;
                    break;
                case 1: // Right
                    newPosition.x += speed;
                    break;
                case 2: // Down
                    newPosition.y += speed;
                    break;
                case 3: // Left
                    newPosition.x -= speed;
                    break;
                }

                // Update bounding box for collision check
                Rectangle newBounds = {
                    newPosition.x + static_cast<float>(GetTileSize()) / 4.0f,
                    newPosition.y + static_cast<float>(GetTileSize()) / 4.0f,
                    static_cast<float>(GetTileSize()) / 2.0f,
                    static_cast<float>(GetTileSize()) / 2.0f};

                // Check if new position is valid
                if (!IsCollidingWithMap(newBounds) &&
                    (!CheckCollisionRecs(newBounds, m_merchantBox) || CheckCollisionRecs(m_playerBoundingBox, m_merchantBox)))
                {
                    m_playerPosition = newPosition;
                }
            }
        }

        // Update bounding box with new position
        m_playerBoundingBox = {
            m_playerPosition.x + static_cast<float>(GetTileSize()) / 4.0f,
            m_playerPosition.y + static_cast<float>(GetTileSize()) / 4.0f,
            static_cast<float>(GetTileSize()) / 2.0f,
            static_cast<float>(GetTileSize()) / 2.0f};

        // Update animation timers with better timing
        if (!m_playerMovementDirections.empty())
        {
            m_playerMotionAnimationTimer += deltaTime * 1000.0f;
            if (m_playerMotionAnimationTimer >= 400.0f)
            {
                m_playerMotionAnimationTimer = 0.0f;
            }

            // Play footstep sounds with better timing
            m_playerFootstepSoundTimer -= deltaTime * 1000.0f;
            if (m_playerFootstepSoundTimer <= 0.0f)
            {
                PlaySound(GetSound("cowboy_footstep"));
                m_playerFootstepSoundTimer = 200.0f;
            }
        }
        else
        {
            // Reset animation timer when not moving
            m_playerMotionAnimationTimer = 0.0f;
        }

        // Check for map transitions
        if (m_waitingForPlayerToMoveDownAMap && m_playerBoundingBox.y + m_playerBoundingBox.height >= 16.0f * GetTileSize() - GetTileSize() / 2)
        {
            SaveGame();
            m_shopping = false;
            m_merchantArriving = false;
            m_merchantLeaving = false;
            m_merchantShopOpen = false;
            m_merchantBox.y = -GetTileSize();
            m_scrollingMap = true;
            // Generate next map once when starting scroll
            GetMap(m_whichWave, m_nextMap);
            m_newMapPosition = 16 * GetTileSize();
            m_temporarySprites.clear();
            m_powerups.clear();
        }
    }

    // Check for monster collisions
    for (int i = m_monsters.size() - 1; i >= 0; i--)
    {
        if (CheckCollisionRecs(m_monsters[i]->position, m_playerBoundingBox) && m_playerInvincibleTimer <= 0)
        {
            if (m_zombieModeTimer <= 0)
            {
                PlayerDie();
                break;
            }
            else if (m_monsters[i]->type != -2) // Not boss
            {
                // Kill monster in zombie mode
                AddGuts({m_monsters[i]->position.x, m_monsters[i]->position.y}, m_monsters[i]->type);
                delete m_monsters[i];
                m_monsters.erase(m_monsters.begin() + i);
                PlaySound(GetSound("Cowboy_monsterDie"));
            }
        }
    }

    // Handle shooting
    if (m_shotTimer > 0)
    {
        m_shotTimer -= static_cast<int>(deltaTime * 1000.0f);
    }

    if (m_deathTimer <= 0.0f && !m_playerShootingDirections.empty() && m_shotTimer <= 0)
    {
        // Special case for spreadfire powerup
        if (m_activePowerups.find(POWERUP_SPREAD) != m_activePowerups.end())
        {
            // Fire in all 8 directions
            std::vector<int> dirs = {0}; // Up
            SpawnBullets(dirs, m_playerPosition);

            dirs = {1}; // Right
            SpawnBullets(dirs, m_playerPosition);

            dirs = {2}; // Down
            SpawnBullets(dirs, m_playerPosition);

            dirs = {3}; // Left
            SpawnBullets(dirs, m_playerPosition);

            // Diagonals
            dirs = {0, 1}; // Up-Right
            SpawnBullets(dirs, m_playerPosition);

            dirs = {1, 2}; // Down-Right
            SpawnBullets(dirs, m_playerPosition);

            dirs = {2, 3}; // Down-Left
            SpawnBullets(dirs, m_playerPosition);

            dirs = {3, 0}; // Up-Left
            SpawnBullets(dirs, m_playerPosition);
        }
        else if (m_playerShootingDirections.size() == 1 ||
                 m_playerShootingDirections.back() == (m_playerShootingDirections[m_playerShootingDirections.size() - 2] + 2) % 4)
        {
            // Single direction or cancelling directions - shoot in the latest direction
            int dirIndex = (m_playerShootingDirections.size() == 2 &&
                            m_playerShootingDirections.back() == (m_playerShootingDirections[m_playerShootingDirections.size() - 2] + 2) % 4)
                               ? 1
                               : 0;

            std::vector<int> dir = {m_playerShootingDirections[dirIndex]};
            SpawnBullets(dir, m_playerPosition);
        }
        else
        {
            // Multiple directions - shoot diagonally
            SpawnBullets(m_playerShootingDirections, m_playerPosition);
        }

        // Play gunshot sound
        PlaySound(GetSound("cowboy_gunshot"));

        // Set shooting cooldown
        m_shotTimer = m_shootingDelay;

        // Apply rapid fire powerup
        if (m_activePowerups.find(POWERUP_RAPIDFIRE) != m_activePowerups.end())
        {
            m_shotTimer /= 4;
        }

        // Apply fire speed level bonus
        for (int i = 0; i < m_fireSpeedLevel; i++)
        {
            m_shotTimer = m_shotTimer * 3 / 4;
        }

        // Apply shotgun penalty
        if (m_activePowerups.find(POWERUP_SHOTGUN) != m_activePowerups.end())
        {
            m_shotTimer = m_shotTimer * 3 / 2;
        }

        // Ensure minimum shot timer
        m_shotTimer = std::max(m_shotTimer, 20);
    }
}

void PrairieKing::Draw()
{
    if (m_onStartMenu)
    {
        // Draw black background
        DrawRectangle(
            static_cast<int>(m_topLeftScreenCoordinate.x),
            static_cast<int>(m_topLeftScreenCoordinate.y),
            16 * GetTileSize(),
            16 * GetTileSize(),
            BLACK);

        // Draw title
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{128.0f, 1744.0f, 96.0f, 56.0f},
            Rectangle{GetScreenWidth() / 2.0f - 3 * GetTileSize(),
                      m_topLeftScreenCoordinate.y + 5 * GetTileSize(),
                      288.0f, 168.0f},
            Vector2{0, 0},
            0.0f,
            WHITE);
    }
    else if ((m_gameOver || m_gameRestartTimer > 0) && !m_endCutscene)
    {
        // Draw black background
        DrawRectangle(
            static_cast<int>(m_topLeftScreenCoordinate.x),
            static_cast<int>(m_topLeftScreenCoordinate.y),
            16 * GetTileSize(),
            16 * GetTileSize(),
            BLACK);

        // Draw game over text
        const char *text = "GAME OVER";
        DrawTextEx(m_assets.GetFont("title"),
                   text,
                   Vector2{m_topLeftScreenCoordinate.x + 6.0f * GetTileSize(),
                           m_topLeftScreenCoordinate.y + 7.0f * GetTileSize()},
                   32,
                   1,
                   WHITE);

        // Draw options
        std::string option1 = "> RETRY";
        std::string option2 = "QUIT";
        if (m_gameOverOption == 1)
        {
            option1 = "RETRY";
            option2 = "> QUIT";
        }

        if (m_gameRestartTimer <= 0 || m_gameRestartTimer / 500 % 2 == 0)
        {
            DrawTextEx(m_assets.GetFont("text"),
                       option1.c_str(),
                       Vector2{m_topLeftScreenCoordinate.x + 6.0f * GetTileSize(),
                               m_topLeftScreenCoordinate.y + 9.0f * GetTileSize()},
                       24,
                       1,
                       WHITE);
        }

        DrawTextEx(m_assets.GetFont("text"),
                   option2.c_str(),
                   Vector2{m_topLeftScreenCoordinate.x + 6.0f * GetTileSize(),
                           m_topLeftScreenCoordinate.y + 9.0f * GetTileSize() + GetTileSize() * 2.0f / 3.0f},
                   24,
                   1,
                   WHITE);
    }
    else
    {
        // Draw the current game map
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                DrawTexturePro(
                    GetTexture("cursors"),
                    Rectangle{464.0f + 16.0f * m_map[x][y] + ((m_map[x][y] == 5 && m_cactusDanceTimer > 800.0f) ? 16.0f : 0.0f),
                              1680.0f - m_world * 16.0f,
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

        // Draw scrolling map if needed - now drawing both maps for proper scrolling effect
        if (m_scrollingMap)
        {
            // Draw next map coming in from below - using the buffered next map
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                for (int y = 0; y < MAP_HEIGHT; y++)
                {
                    DrawTexturePro(
                        GetTexture("cursors"),
                        Rectangle{464.0f + 16.0f * m_nextMap[x][y] + ((m_nextMap[x][y] == 5 && m_cactusDanceTimer > 800.0f) ? 16.0f : 0.0f),
                                  1680.0f - m_world * 16.0f,
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

        // Draw control instructions at beginning of game
        if (m_betweenWaveTimer > 0 && m_whichWave == 0 && !m_scrollingMap)
        {
            Vector2 pos = {GetScreenWidth() / 2 - 120, GetScreenHeight() - 144 - 3};
            
            // Drawing controls instruction box
            DrawTexturePro(
                GetTexture("cursors"),
                Rectangle{352, 1648, 80, 48},
                Rectangle{pos.x, pos.y, 240, 144}, // 3x scaling
                Vector2{0, 0},
                0.0f,
                WHITE);
                    }

        // Draw player if not dead or invincible
        if (m_deathTimer <= 0.0f && (m_playerInvincibleTimer <= 0 || m_playerInvincibleTimer / 100 % 2 == 0))
        {
            if (m_holdItemTimer > 0)
            {
                // Draw player holding item
                DrawTexturePro(
                    GetTexture("cursors"),
                    Rectangle{384.0f, 1760.0f, 16.0f, 16.0f},
                    Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                              m_topLeftScreenCoordinate.y + m_playerPosition.y - GetTileSize() / 4,
                              48.0f, 48.0f},
                    Vector2{0, 0},
                    0.0f,
                    WHITE);

                // Draw held item above player
                DrawTexturePro(
                    GetTexture("cursors"),
                    Rectangle{320.0f + m_itemToHold * 16.0f, 1776.0f, 16.0f, 16.0f},
                    Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                              m_topLeftScreenCoordinate.y + m_playerPosition.y - GetTileSize() * 2 / 3 - GetTileSize() / 4,
                              48.0f, 48.0f},
                    Vector2{0, 0},
                    0.0f,
                    WHITE);
            }
            else if (m_zombieModeTimer > 0)
            {
                // Draw zombie player
                DrawTexturePro(
                    GetTexture("cursors"),
                    Rectangle{352.0f + ((m_zombieModeTimer / 50 % 2) * 16.0f), 1760.0f, 16.0f, 16.0f},
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
                    Rectangle{496.0f, 1760.0f, 16.0f, 16.0f},
                    Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                              m_topLeftScreenCoordinate.y + m_playerPosition.y - GetTileSize() / 4,
                              48.0f, 48.0f},
                    Vector2{0, 0},
                    0.0f,
                    WHITE);
            }
            else
            {
                // Get facing direction from movement or shooting
                int facingDirection = m_playerShootingDirections.empty() ? (!m_playerMovementDirections.empty() ? m_playerMovementDirections[0] : 0) : m_playerShootingDirections.back();

                // Draw player feet - fixed the animation coordinates and timing
                int footFrame = static_cast<int>(m_playerMotionAnimationTimer / 100.0f) % 4;
                DrawTexturePro(
                    GetTexture("cursors"),
                    Rectangle{483.0f, 1760.0f + footFrame * 3, 10.0f, 3.0f},
                    Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x + 9.0f, // Changed from 18.0f to 9.0f
                              m_topLeftScreenCoordinate.y + m_playerPosition.y + 39.0f,
                              30.0f, 9.0f},
                    Vector2{0, 0},
                    0.0f,
                    WHITE);
                // Draw player body - now using a cleaner sprite
                DrawTexturePro(
                    GetTexture("cursors"),
                    Rectangle{464.0f + facingDirection * 16.0f, 1744.0f, 16.0f, 16.0f},
                    Rectangle{m_topLeftScreenCoordinate.x + m_playerPosition.x,
                              m_topLeftScreenCoordinate.y + m_playerPosition.y,
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

        // Draw temporary sprites
        for (auto &sprite : m_temporarySprites)
        {
            sprite.Draw(GetTexture("cursors"));
        }

        // Draw bullets with crisper rendering
        for (const auto &bullet : m_bullets)
        {
            DrawTexturePro(
                GetTexture("cursors"),
                Rectangle{518.0f, 1760.0f + (m_bulletDamage - 1) * 4.0f, 4.0f, 4.0f},
                Rectangle{m_topLeftScreenCoordinate.x + bullet.position.x,
                          m_topLeftScreenCoordinate.y + bullet.position.y,
                          12.0f, 12.0f}, // Slightly smaller for crisper appearance
                Vector2{0, 0},
                0.0f,
                WHITE);
        }

        // Draw enemy bullets
        for (const auto &bullet : m_enemyBullets)
        {
            DrawTexturePro(
                GetTexture("cursors"),
                Rectangle{523.0f, 1760.0f, 5.0f, 5.0f},
                Rectangle{m_topLeftScreenCoordinate.x + bullet.position.x,
                          m_topLeftScreenCoordinate.y + bullet.position.y,
                          15.0f, 15.0f},
                Vector2{0, 0},
                0.0f,
                WHITE);
        }

        // Draw UI elements
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{294.0f, 1782.0f, 22.0f, 22.0f},
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
                Rectangle{272.0f + m_heldItem->which * 16.0f, 1808.0f, 16.0f, 16.0f},
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
            Rectangle{400.0f, 1776.0f, 16.0f, 16.0f},
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
            Rectangle{272.0f, 1808.0f, 16.0f, 16.0f},
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
                Rectangle{512.0f, 1760.0f, 5.0f, 5.0f},
                Rectangle{m_topLeftScreenCoordinate.x + GetTileSize() * 16 + 3,
                          static_cast<float>(i * 3 * 6),
                          15.0f, 15.0f},
                Vector2{0, 0},
                0.0f,
                WHITE);
        }

        // Draw timer bar
        DrawTexturePro(
            GetTexture("cursors"),
            Rectangle{595.0f, 1748.0f, 9.0f, 11.0f},
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
                Rectangle{416.0f + (m_ammoLevel - 1) * 16.0f, 1776.0f, 16.0f, 16.0f},
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
                Rectangle{320.0f + (m_fireSpeedLevel - 1) * 16.0f, 1776.0f, 16.0f, 16.0f},
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
                Rectangle{368.0f + (m_runSpeedLevel - 1) * 16.0f, 1776.0f, 16.0f, 16.0f},
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
                Rectangle{464.0f, 1776.0f, 16.0f, 16.0f},
                Rectangle{m_topLeftScreenCoordinate.x - GetTileSize() - 3,
                          m_topLeftScreenCoordinate.y + 16 * GetTileSize() - GetTileSize() * 4,
                          48.0f, 48.0f},
                Vector2{0, 0},
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
                Rectangle{355.0f, 1750.0f, 8.0f, 8.0f},
                Rectangle{m_topLeftScreenCoordinate.x + 8.5f * GetTileSize() - 12,
                        m_topLeftScreenCoordinate.y + 15.0f * GetTileSize(),
                        24.0f, 24.0f},
                Vector2{0, 0},
                0.0f,
                WHITE);
        }
    }
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

void PrairieKing::SetButtonState(GameKeys key, bool pressed)
{
    if (pressed)
    {
        m_buttonHeldState.insert(key);
    }
    else
    {
        m_buttonHeldState.erase(key);
    }
}

void PrairieKing::AddMonster(CowboyMonster *monster)
{
    if (monster != nullptr)
    {
        m_monsters.push_back(monster);
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
    m_whichWave++;
    if (m_whichWave > 0)
    {
        if (m_whichWave % 4 == 0)
        {
            StartShoppingLevel();
        }
        else
        {
            m_waitingForPlayerToMoveDownAMap = true;
            // Open a path at the bottom of the map
            m_map[8][15] = MAP_DESERT;
            m_map[7][15] = MAP_DESERT;
            m_map[9][15] = MAP_DESERT;
        }
    }
}