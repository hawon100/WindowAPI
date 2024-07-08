#include "ConsoleGameEngine.h"
#include <random>

class VerticalScrollerShooter : public ConsoleGameEngine
{
public:
    VerticalScrollerShooter()
    {
        m_sAppName = L"Vertical Scroller Shooter";
    }

private:
    // Game state variables
    float playerPosX = 40.0f;
    float playerPosY = 28.0f;
    int playerHP = 3;
    int score = 0;

    std::vector<std::pair<float, float>> playerBullets;
    std::vector<std::pair<float, float>> enemyBullets;
    std::vector<std::pair<float, float>> enemies;
    float enemySpawnTimer = 0.0f;
    float enemyShootTimer = 0.0f;
    float playerFireTimer = 0.0f;

    // Game settings
    float playerSpeed = 20.0f;
    float bulletSpeed = 50.0f;
    float enemySpeed = 10.0f;
    float enemyBulletSpeed = 30.0f;
    float spawnInterval = 2.0f;
    float shootInterval = 1.0f;
    float playerFireInterval = 0.2f; // Time interval between shots when holding space bar
    float enemyFollowSpeed = 5.0f;   // Speed at which enemies follow the player

    bool gameOver = false;
    float gameOverTimer = 0.0f;

    std::default_random_engine rng;
    std::uniform_int_distribution<int> spawnCountDist{ 1, 5 };
    std::uniform_int_distribution<int> spawnOffsetDist{ -5, 5 };

protected:
    virtual bool OnUserCreate() override
    {
        // Initialization
        rng.seed(std::random_device{}());
        ResetGame();
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        if (gameOver)
        {
			DrawString(ScreenWidth() / 2 - 7, ScreenHeight() / 2, L"Game over!", FG_RED);
            // Show Game Over message for 5 seconds
            gameOverTimer += fElapsedTime;
            if (gameOverTimer >= 5.0f)
            {
                DrawString(ScreenWidth() / 2 - 10, ScreenHeight() / 2 + 1, L"Press 'R' to Restart", FG_RED);
                if (GetKey('R').bPressed)
                {
                    ResetGame();
                    gameOver = false;
                }
            }
            return true;
        }

        // Player Movement
        if (GetKey(VK_LEFT).bHeld && playerPosX > 0)
            playerPosX -= playerSpeed * fElapsedTime;
        if (GetKey(VK_RIGHT).bHeld && playerPosX < ScreenWidth() - 1)
            playerPosX += playerSpeed * fElapsedTime;

        // Shooting
        playerFireTimer += fElapsedTime;
        if (GetKey(VK_SPACE).bHeld && playerFireTimer >= playerFireInterval)
        {
            playerBullets.push_back({ playerPosX, playerPosY - 1 });
            playerFireTimer = 0.0f;
        }

        // Update Player Bullets
        for (auto& bullet : playerBullets)
            bullet.second -= bulletSpeed * fElapsedTime;
        playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(),
            [&](std::pair<float, float>& b) { return b.second < 0; }), playerBullets.end());

        // Spawn Enemies
        enemySpawnTimer += fElapsedTime;
        if (enemySpawnTimer >= spawnInterval)
        {
            int enemyCount = spawnCountDist(rng);
            std::vector<int> spawnPositions;

            while (spawnPositions.size() < enemyCount)
            {
                int offset = spawnOffsetDist(rng);
                int spawnX = static_cast<int>(playerPosX) + offset;
                if (spawnX >= 0 && spawnX < ScreenWidth() &&
                    std::find(spawnPositions.begin(), spawnPositions.end(), spawnX) == spawnPositions.end())
                {
                    spawnPositions.push_back(spawnX);
                    enemies.push_back({ static_cast<float>(spawnX), 0.0f });
                }
            }

            enemySpawnTimer -= spawnInterval;
        }

        // Update Enemies
        for (auto& enemy : enemies)
        {
            // Move enemy downwards
            enemy.second += enemySpeed * fElapsedTime;

            // Move enemy towards player's x-coordinate
            if (enemy.first < playerPosX)
                enemy.first += enemyFollowSpeed * fElapsedTime;
            else if (enemy.first > playerPosX)
                enemy.first -= enemyFollowSpeed * fElapsedTime;
        }

        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
            [&](std::pair<float, float>& e) { return e.second >= ScreenHeight(); }), enemies.end());

        // Enemy Shooting
        enemyShootTimer += fElapsedTime;
        if (enemyShootTimer >= shootInterval)
        {
            for (auto& enemy : enemies)
                enemyBullets.push_back({ enemy.first, enemy.second + 1 });
            enemyShootTimer -= shootInterval;
        }

        // Update Enemy Bullets
        for (auto& bullet : enemyBullets)
            bullet.second += enemyBulletSpeed * fElapsedTime;
        enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
            [&](std::pair<float, float>& b) { return b.second >= ScreenHeight(); }), enemyBullets.end());

        // Collision Detection for Player Bullets and Enemies
        for (auto it = enemies.begin(); it != enemies.end();)
        {
            bool hit = false;
            for (auto bt = playerBullets.begin(); bt != playerBullets.end();)
            {
                if (abs(it->first - bt->first) < 1.0f && abs(it->second - bt->second) < 1.0f)
                {
                    bt = playerBullets.erase(bt);
                    hit = true;
                    score += 10; // Increase score for each enemy hit
                }
                else
                {
                    ++bt;
                }
            }
            if (hit)
                it = enemies.erase(it);
            else
                ++it;
        }

        // Collision Detection for Enemy Bullets and Player
        for (auto it = enemyBullets.begin(); it != enemyBullets.end();)
        {
            if (abs(it->first - playerPosX) < 1.0f && abs(it->second - playerPosY) < 1.0f)
            {
                it = enemyBullets.erase(it);
                playerHP -= 1; // Decrease player HP for each hit
                if (playerHP <= 0)
                {
                    // Game Over
                    gameOver = true;
                    gameOverTimer = 0.0f;
                    Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ', 0);
                    DrawString(ScreenWidth() / 2 - 5, ScreenHeight() / 2, L"GAME OVER", FG_RED);
                }
            }
            else
            {
                ++it;
            }
        }

        // Draw Everything
        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ', 0);

        // Draw Player
        Draw(playerPosX, playerPosY, PIXEL_SOLID, FG_WHITE);

        // Draw Player Bullets
        for (auto& bullet : playerBullets)
            Draw(bullet.first, bullet.second, PIXEL_SOLID, FG_YELLOW);

        // Draw Enemies
        for (auto& enemy : enemies)
            Draw(enemy.first, enemy.second, PIXEL_SOLID, FG_RED);

        // Draw Enemy Bullets
        for (auto& bullet : enemyBullets)
            Draw(bullet.first, bullet.second, PIXEL_SOLID, FG_CYAN);

        // Draw HP and Score
        DrawString(0, 0, L"HP: " + std::to_wstring(playerHP), FG_WHITE);
        DrawString(0, 1, L"Score: " + std::to_wstring(score), FG_WHITE);

        return true;
    }

    void ResetGame()
    {
        playerPosX = ScreenWidth() / 2.0f;
        playerPosY = ScreenHeight() - 2.0f;
        playerHP = 3;
        score = 0;
        playerBullets.clear();
        enemyBullets.clear();
        enemies.clear();
        enemySpawnTimer = 0.0f;
        enemyShootTimer = 0.0f;
        playerFireTimer = 0.0f;
        gameOverTimer = 0.0f;
    }
};

int main()
{
    VerticalScrollerShooter game;
    if (game.ConstructConsole(80, 30, 8, 8))
        game.Start();
    return 0;
}
