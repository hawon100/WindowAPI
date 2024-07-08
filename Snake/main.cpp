#include "ConsoleGameEngine.h"
#include <random>
#include <deque>

class SnakeGame : public ConsoleGameEngine
{
public:
    SnakeGame()
    {
        m_sAppName = L"Snake Game";
    }

private:
    enum Direction
    {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    struct Segment
    {
        int x;
        int y;
    };

    std::deque<Segment> snake;
    int appleX;
    int appleY;
    Direction direction;
    int score;
    bool gameOver;
    float moveTimer;
    float moveInterval;
    std::default_random_engine rng;

protected:
    virtual bool OnUserCreate() override
    {
        rng.seed(std::random_device{}());
        ResetGame();
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        if (gameOver)
        {
			DrawString(ScreenWidth() / 2 - 7, ScreenHeight() / 2, L"Game Over!", FG_RED);
            // Show Game Over message for 3 seconds
            moveTimer += fElapsedTime;
            if (moveTimer >= 3.0f)
            {
                DrawString(ScreenWidth() / 2 - 10, ScreenHeight() / 2 + 1, L"Press 'R' to Restart", FG_RED);
                if (GetKey('R').bPressed)
                {
                    ResetGame();
                }
            }
            return true;
        }

        // Input handling for direction
        if (GetKey(VK_UP).bPressed && direction != DOWN) direction = UP;
        if (GetKey(VK_DOWN).bPressed && direction != UP) direction = DOWN;
        if (GetKey(VK_LEFT).bPressed && direction != RIGHT) direction = LEFT;
        if (GetKey(VK_RIGHT).bPressed && direction != LEFT) direction = RIGHT;

        moveTimer += fElapsedTime;
        if (moveTimer >= moveInterval)
        {
            moveTimer = 0.0f;
            // Move the snake
            MoveSnake();

            // Check for collisions
            if (CheckCollision())
            {
                gameOver = true;
                moveTimer = 0.0f;
                Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ', 0);
                DrawString(ScreenWidth() / 2 - 5, ScreenHeight() / 2, L"GAME OVER", FG_RED);
            }

            // Check if snake ate the apple
            if (snake.front().x == appleX && snake.front().y == appleY)
            {
                score += 10;
                GrowSnake();
                PlaceApple();
            }
        }

        // Draw Everything
        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ', 0);

        // Draw Snake
        for (const auto& segment : snake)
        {
            Draw(segment.x, segment.y, PIXEL_SOLID, FG_GREEN);
        }

        // Draw Apple
        Draw(appleX, appleY, PIXEL_SOLID, FG_RED);

        // Draw Score
        DrawString(0, 0, L"Score: " + std::to_wstring(score), FG_WHITE);

        return true;
    }

    void ResetGame()
    {
        snake.clear();
        snake.push_back({ ScreenWidth() / 2, ScreenHeight() / 2 });
        direction = RIGHT;
        score = 0;
        gameOver = false;
        moveTimer = 0.0f;
        moveInterval = 0.1f; // Speed of the snake
        PlaceApple();
    }

    void MoveSnake()
    {
        Segment newHead = snake.front();
        switch (direction)
        {
        case UP: newHead.y -= 1; break;
        case DOWN: newHead.y += 1; break;
        case LEFT: newHead.x -= 1; break;
        case RIGHT: newHead.x += 1; break;
        }
        snake.push_front(newHead);
        snake.pop_back();
    }

    void GrowSnake()
    {
        Segment newTail = snake.back();
        snake.push_back(newTail);
    }

    bool CheckCollision()
    {
        const Segment& head = snake.front();

        // Check wall collision
        if (head.x < 0 || head.x >= ScreenWidth() || head.y < 0 || head.y >= ScreenHeight())
            return true;

        // Check self collision
        for (size_t i = 1; i < snake.size(); ++i)
        {
            if (snake[i].x == head.x && snake[i].y == head.y)
                return true;
        }

        return false;
    }

    void PlaceApple()
    {
        std::uniform_int_distribution<int> distX(0, ScreenWidth() - 1);
        std::uniform_int_distribution<int> distY(0, ScreenHeight() - 1);

        while (true)
        {
            appleX = distX(rng);
            appleY = distY(rng);

            bool collidesWithSnake = false;
            for (const auto& segment : snake)
            {
                if (segment.x == appleX && segment.y == appleY)
                {
                    collidesWithSnake = true;
                    break;
                }
            }

            if (!collidesWithSnake)
                break;
        }
    }
};

int main()
{
    SnakeGame game;
    if (game.ConstructConsole(80, 30, 8, 8))
        game.Start();
    return 0;
}
