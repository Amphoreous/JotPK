#include "gameplay/Monster.hpp"
#include "gameplay/PrairieKing.hpp"
#include <cmath>

CowboyMonster::CowboyMonster(AssetManager& assets, int which, int health, int speed, Vector2 position)
    : PrairieKing(assets), health(health), type(which), speed(speed),
    position({ position.x, position.y, static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM), static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) }),
    movementDirection(0), movedLastTurn(false), oppositeMotionGuy(false),
    invisible(false), special(false), uninterested(false), flyer(false),
    tint(WHITE), flashColor(RED), flashColorTimer(0), ticksSinceLastMovement(0),
    acceleration({ 0, 0 }), targetPosition({ 0, 0 }), movementAnimationTimer(0)
{
    uninterested = (rand() % 100 < 25);
}

CowboyMonster::CowboyMonster(AssetManager& assets, int which, Vector2 position)
    : PrairieKing(assets), type(which),
    position({ position.x, position.y, static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM), static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) }),
    movementDirection(0), movedLastTurn(false), oppositeMotionGuy(false),
    invisible(false), special(false), uninterested(false), flyer(false),
    tint(WHITE), flashColor(RED), flashColorTimer(0), ticksSinceLastMovement(0),
    acceleration({ 0, 0 }), targetPosition({ 0, 0 }), movementAnimationTimer(0)
{
    switch (type)
    {
    case ORC:
        speed = ORC_SPEED;
        health = ORC_HEALTH;
        uninterested = (rand() % 100 < 25);
        if (uninterested)
        {
            targetPosition = { static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM),
                              static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) };
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
            targetPosition = { static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM),
                              static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) };
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
            targetPosition = { static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM),
                              static_cast<float>(rand() % 12 + 2) * static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) };
            tries++;
        } while (false); // We'll fix this collision check later
        break;
    }
    }
    oppositeMotionGuy = (rand() % 100 < 50);
}

void CowboyMonster::Draw(const Texture2D& texture, Vector2 topLeftScreenCoordinate)
{
    if (type == SPIKEY && special)
    {
        Rectangle sourceRect;
        if (flashColorTimer > 0.0f)
        {
            sourceRect = { 352.0f, 48.0f, 16.0f, 16.0f };
        }
        else
        {
            sourceRect = { 448.0f, 64.0f, 16.0f, 16.0f };
        }

        DrawTexturePro(
            texture,
            sourceRect,
            Rectangle{ topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height },
            Vector2{ 0, 0 },
            0.0f,
            WHITE);
    }
    else if (!invisible)
    {
        Rectangle sourceRect;
        if (flashColorTimer > 0.0f)
        {
            sourceRect = { 224.0f + type * 16.0f, 48.0f, 16.0f, 16.0f };
        }
        else
        {
            int frame = (movementAnimationTimer < 250.0f) ? 1 : 0;
            sourceRect = { 224.0f + (type * 2 + frame) * 16.0f, 64.0f, 16.0f, 16.0f };
        }

        DrawTexturePro(
            texture,
            sourceRect,
            Rectangle{ topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height },
            Vector2{ 0, 0 },
            0.0f,
            WHITE);
    }
}

bool CowboyMonster::TakeDamage(int damage)
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

int CowboyMonster::GetLootDrop()
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

bool CowboyMonster::Move(Vector2 playerPosition, float deltaTime)
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
                    static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) }) && tries < 5);
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
            target = { gopherBox.x, gopherBox.y };
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
                Vector2{ target.x + static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) / 2,
                      target.y + static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) / 2 },
                position))
            {
                targetPosition = { 0, 0 };

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
                        targetPosition = { 0, 0 }; // Clear target to focus on player
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
                Vector2{ target.x + static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) / 2,
                       target.y + static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) / 2 },
                position))
            {
                targetPosition = { 0, 0 };
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
                static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) }) && tries < 5);

            // Create animated spike sprite
            if (PrairieKing::GetGameInstance() && !invisible)
            {
                // Add temporary sprite animation for spike formation
                PrairieKing::TemporaryAnimatedSprite spike(
                    Rectangle{ 224, 64, 16, 16 },
                    60.0f, 3, 0,
                    Vector2{ position.x + PrairieKing::GetGameInstance()->m_topLeftScreenCoordinate.x, position.y + PrairieKing::GetGameInstance()->m_topLeftScreenCoordinate.y },
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

void CowboyMonster::SpikeyEndBehavior(int extraInfo)
{
    invisible = false;
    health += 5;
    special = true;
}

// Implementation of the Dracula class
Dracula::Dracula(AssetManager& assets)
    : CowboyMonster(assets, -2, Vector2{ static_cast<float>(8 * BASE_TILE_SIZE * PIXEL_ZOOM),
                                static_cast<float>(8 * BASE_TILE_SIZE * PIXEL_ZOOM) })
{
    homePosition = { position.x, position.y };
    position.y += static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM * 4);
    health = 350;
    fullHealth = health;
    phase = GLOATING_PHASE;
    phaseInternalTimer = 4000;
    speed = 2;
}

void Dracula::Draw(const Texture2D& texture, Vector2 topLeftScreenCoordinate)
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
            Rectangle{ 336.0f, 48.0f, 16.0f, 16.0f },
            Rectangle{ topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height },
            Vector2{ 0, 0 },
            0.0f,
            WHITE);
        return;
    }

    if (phase == GLOATING_PHASE || phase == SPREAD_SHOT_PHASE ||
        phase == SUMMON_DEMON_PHASE || phase == SUMMON_MUMMY_PHASE)
    {
        DrawTexturePro(
            texture,
            Rectangle{ 464.0f + (phaseInternalTimer / 100 % 3) * 16.0f, 112.0f, 16.0f, 16.0f },
            Rectangle{ topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height },
            Vector2{ 0, 0 },
            0.0f,
            WHITE);

        if (phase == GLOATING_PHASE)
        {
            // Draw floating effect
            DrawTexturePro(
                texture,
                Rectangle{ 400.0f, 1776.0f, 16.0f, 16.0f },
                Rectangle{
                    topLeftScreenCoordinate.x + position.x,
                    topLeftScreenCoordinate.y + position.y + position.height + sinf(phaseInternalTimer / 1000.0f) * 3,
                    position.width, position.height },
                    Vector2{ 0, 0 },
                    0.0f,
                    WHITE);

            // Draw crown/hat
            DrawTexturePro(
                texture,
                Rectangle{ 608.0f, 1728.0f, 32.0f, 32.0f },
                Rectangle{
                    topLeftScreenCoordinate.x + position.x - static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM) / 2.0f,
                    topLeftScreenCoordinate.y + position.y - static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM * 2),
                    32.0f * 3.0f, 32.0f * 3.0f },
                    Vector2{ 0, 0 },
                    0.0f,
                    WHITE);
        }
    }
    else
    {
        DrawTexturePro(
            texture,
            Rectangle{ 592.0f + (phaseInternalTimer / 100 % 2) * 16.0f, 1712.0f, 16.0f, 16.0f },
            Rectangle{ topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height },
            Vector2{ 0, 0 },
            0.0f,
            WHITE);
    }
}

int Dracula::GetLootDrop()
{
    return POWERUP_LOG;
}

bool Dracula::TakeDamage(int damage)
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

bool Dracula::Move(Vector2 playerPosition, float deltaTime)
{
    if (flashColorTimer > 0.0f)
    {
        flashColorTimer -= deltaTime * 1000.0f;
    }

    phaseInternalTimer -= deltaTime * 1000.0f;

    // Simplified implementation for now
    return false;
}

void Dracula::FireSpread(Vector2 origin, double offsetAngle)
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
            sinf(angle) * 6.0f };

        // Bullets will be added in the final implementation
    }

    PlaySound(LoadSound("Cowboy_gunshot"));
}

void Dracula::SummonEnemies(Vector2 origin, int which)
{
    int tileSize = BASE_TILE_SIZE * PIXEL_ZOOM;

    Rectangle leftSpawn = { origin.x - static_cast<float>(tileSize) - static_cast<float>(tileSize) / 2.0f, origin.y,
                           static_cast<float>(tileSize), static_cast<float>(tileSize) };
    Rectangle rightSpawn = { origin.x + static_cast<float>(tileSize) + static_cast<float>(tileSize) / 2.0f, origin.y,
                            static_cast<float>(tileSize), static_cast<float>(tileSize) };
    Rectangle bottomSpawn = { origin.x, origin.y + static_cast<float>(tileSize) + static_cast<float>(tileSize) / 2.0f,
                             static_cast<float>(tileSize), static_cast<float>(tileSize) };
    Rectangle topSpawn = { origin.x, origin.y - static_cast<float>(tileSize) - static_cast<float>(tileSize) * 3.0f / 4.0f,
                          static_cast<float>(tileSize), static_cast<float>(tileSize) };

    PlaySound(LoadSound("Cowboy_monsterDie"));
}

// Implementation of the Outlaw class
Outlaw::Outlaw(AssetManager& assets, Vector2 position, int health)
    : CowboyMonster(assets, -1, position)
{
    homePosition = { position.x, position.y };
    this->health = health;
    fullHealth = health;
    phaseCountdown = 4000;
    phase = TALKING_PHASE;
}

void Outlaw::Draw(const Texture2D& texture, Vector2 topLeftScreenCoordinate)
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
            Rectangle{ 496.0f, 1696.0f, 16.0f, 16.0f },
            Rectangle{ topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height },
            Vector2{ 0, 0 },
            0.0f,
            WHITE);
        return;
    }

    if (phase == TALKING_PHASE || phase == HIDING_PHASE)
    {
        DrawTexturePro(
            texture,
            Rectangle{ 560.0f + ((phaseCountdown / 250 % 2 == 0) ? 16.0f : 0.0f), 1776.0f, 16.0f, 16.0f },
            Rectangle{ topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height },
            Vector2{ 0, 0 },
            0.0f,
            WHITE);

        if (phase == TALKING_PHASE && phaseCountdown > 1000)
        {
            int whichWave = PrairieKing::GetGameInstance()->m_whichWave;
            // Draw speech bubble
            DrawTexturePro(
                texture,
                Rectangle{ 576.0f + ((whichWave > 5) ? 32.0f : 0.0f), 1792.0f, 32.0f, 32.0f },
                Rectangle{
                    topLeftScreenCoordinate.x + position.x - static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM / 2),
                    topLeftScreenCoordinate.y + position.y - static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM * 2),
                    static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM * 3),
                    static_cast<float>(BASE_TILE_SIZE * PIXEL_ZOOM * 3) },
                    Vector2{ 0, 0 },
                    0.0f,
                    WHITE);
        }
    }
    else if (phase == RUN_GUN_AND_PANT_PHASE && phaseInternalCounter == 2)
    {
        DrawTexturePro(
            texture,
            Rectangle{ 560.0f + ((phaseCountdown / 250 % 2 == 0) ? 16.0f : 0.0f), 1776.0f, 16.0f, 16.0f },
            Rectangle{ topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height },
            Vector2{ 0, 0 },
            0.0f,
            WHITE);
    }
    else
    {
        DrawTexturePro(
            texture,
            Rectangle{ 592.0f + ((phaseCountdown / 80 % 2 == 0) ? 16.0f : 0.0f), 1776.0f, 16.0f, 16.0f },
            Rectangle{ topLeftScreenCoordinate.x + position.x, topLeftScreenCoordinate.y + position.y, position.width, position.height },
            Vector2{ 0, 0 },
            0.0f,
            WHITE);
    }
}

bool Outlaw::TakeDamage(int damage)
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

int Outlaw::GetLootDrop()
{
    return POWERUP_HEART;
}

bool Outlaw::Move(Vector2 playerPosition, float deltaTime)
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
                Vector2 bulletSpawnPosition = { position.x + position.width / 2, position.y + position.height / 2 };

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
                Vector2 bulletVelocity = { direction.x * 6.0f, direction.y * 6.0f };

                // Add to enemy bullets - will be done through main game instance
                PrairieKing* instance = PrairieKing::GetGameInstance();
                if (instance)
                {
                    // Add main bullet
                    instance->m_enemyBullets.push_back(CowboyBullet(bulletSpawnPosition, bulletVelocity, 1));

                    // Add slightly off-angle bullets
                    float angle = atan2f(bulletVelocity.y, bulletVelocity.x);
                    float spread = 0.2f; // 0.2 radians spread

                    Vector2 bulletVelocity1 = {
                        cosf(angle + spread) * 6.0f,
                        sinf(angle + spread) * 6.0f };

                    Vector2 bulletVelocity2 = {
                        cosf(angle - spread) * 6.0f,
                        sinf(angle - spread) * 6.0f };

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